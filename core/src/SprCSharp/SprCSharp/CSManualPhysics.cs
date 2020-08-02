﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace SprCs {
    public partial class PHSceneIf : SceneIf {
        public bool isStepping = false;
        public bool isFixedUpdating = false;
        public bool isSwapping = false;
        public bool threadMode = false;
        public bool show_this2 = false;
        public bool fixedUpdateFinished = true;
        public readonly object phSceneForGetSetLock = new object();
        public int sceneForStep = 0;
        public int sceneForGet = 1;
        public int sceneForBuffer = 2;

        public static Dictionary<IntPtr, PHSceneIf> instances = new Dictionary<IntPtr, PHSceneIf>();
        private List<ThreadCallback> waitDuringStepCallbackList = new List<ThreadCallback>();
        private ObjectStatesIf state = ObjectStatesIf.Create();
        public Dictionary<System.Object, bool> isSteppedFlagDictionary = new Dictionary<System.Object, bool>(); // インスタンスごとにステップ済みかのフラグを用意，trueにするのはTimer内，falseにするのは各インスタンス
        public readonly AutoResetEvent fixedUpdateAutoWait = new AutoResetEvent(false);
        public bool allSteppedFlagsFalse = false;
        public bool changeAllSteppedFlagsTrue = false;
        public bool firstGetIsSteppedFlagInOneFixedUpdate = true; // FixedUpdateのDefaultExecutionOrderを使わないための苦肉の策
        // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
        // Thread処理用
        public delegate void ThreadCallback();
        //public List<ThreadCallback> waitUntilNextStepCallbackList = new List<ThreadCallback>();
        public void ExecWaitUntilNextStepCallbackList() { // Step側でLockする
            if (waitDuringStepCallbackList.Count != 0) {
                Console.WriteLine("waitduringstepcallbacklist " + waitDuringStepCallbackList.Count);
            }
            foreach (var callback in waitDuringStepCallbackList) {
                callback();
            }
            waitDuringStepCallbackList.Clear();
        }
        public void AddWaitUntilNextStepCallback(ThreadCallback callback) {
            waitDuringStepCallbackList.Add(callback);
        }
        public static PHSceneIf CreateCSInstance(IntPtr stepPHScene, IntPtr bufferPHScene, IntPtr getPHScene) { // 次にStepされるPHSceneIfを返す
            if (!instances.ContainsKey(stepPHScene)) { // defaultIntPtrをinstances[defaultIntPtr]._thisに代入
                var newPHSceneIf = new PHSceneIf(stepPHScene);
                instances[stepPHScene] = newPHSceneIf;
                instances[bufferPHScene] = newPHSceneIf;
                instances[getPHScene] = newPHSceneIf;
                instances[stepPHScene]._thisArray[0] = stepPHScene;
            }
            instances[stepPHScene]._thisArray[1] = bufferPHScene;
            if (instances[stepPHScene]._thisArray[1] == IntPtr.Zero) {
                Console.WriteLine("instances[stepPHScene]._thisArray[1] null");
            } else {
                Console.WriteLine("instances[stepPHScene]._thisArray[1] not null");
            }
            instances[stepPHScene]._thisArray[2] = getPHScene;
            return instances[stepPHScene];
        }
        public static PHSceneIf GetCSInstance(IntPtr intptr1) { // 引数にはGetSceneしてきた_thisを入れる
            PHSceneIf defaultPHSceneIf = null;
            if (instances.ContainsKey(intptr1)) {
                defaultPHSceneIf = instances[intptr1];
                return defaultPHSceneIf;
            }
            return null;
        }
        //public IntPtr GetNotNextStepPHScene() { // GetやSetが使用
        //    lock (phSceneForGetSetLock) {
        //        if (step_thisOnNext) {
        //            return _this;
        //        } else {
        //            return _this2;
        //        }
        //    }
        //}
        public void Step() {
            if (threadMode) {
                lock (phSceneForGetSetLock) {
                    isStepping = true;
                    changeAllSteppedFlagsTrue = true;
                }
                SprExport.Spr_PHSceneIf_Step(_thisArray[sceneForStep]);
            } else {
                if (show_this2) {
                    SprExport.Spr_PHSceneIf_Step(_this2);
                    SprExport.Spr_ObjectStatesIf_SaveState(state._this, _this2); // _this2→state
                    SprExport.Spr_ObjectStatesIf_LoadState(state._this, _this); // state→_this
                } else {
                    SprExport.Spr_PHSceneIf_Step(_this);
                }
            }
        }
        public void Swap() {
            lock (phSceneForGetSetLock) {
                ExecWaitUntilNextStepCallbackList(); // NotnextStepPHSceneを呼ぶとSave/Load後に呼ばれるべきCallbackが先に実行されてしまう
                SprExport.Spr_ObjectStatesIf_SaveState(state._this, _thisArray[sceneForStep]); // phScene→state
                if (!isFixedUpdating) { // Step↔Get
                    SprExport.Spr_ObjectStatesIf_LoadState(state._this, _thisArray[sceneForGet]); // state→phScene
                    var temp = sceneForStep;
                    sceneForStep = sceneForGet;
                    sceneForGet = temp;
                } else { // Step↔Buffer
                    isSwapping = true;
                    SprExport.Spr_ObjectStatesIf_LoadState(state._this, _thisArray[sceneForBuffer]); // state→phScene
                    var temp = sceneForStep;
                    sceneForStep = sceneForBuffer;
                    sceneForBuffer = temp;
                }
                isStepping = false;

            }
        }
        public void SwapAfterFixedUpdate() {
            lock (phSceneForGetSetLock) {
                if (isSwapping) { // Buffer↔Get
                    var temp = sceneForBuffer;
                    sceneForBuffer = sceneForGet;
                    sceneForGet = temp;
                    isSwapping = false;
                }
                isFixedUpdating = false;
            }
        }
        public bool GetIsSteppedFlag(System.Object o) {
            lock (phSceneForGetSetLock) {
                if (!isSteppedFlagDictionary.ContainsKey(o)) {
                    return isSteppedFlagDictionary[o] = true; // 恐らく初期値trueで問題ない，制御されてないのにStepされても困るし
                } else {
                    if (changeAllSteppedFlagsTrue && firstGetIsSteppedFlagInOneFixedUpdate) {
                        List<System.Object> Keys = new List<object>(isSteppedFlagDictionary.Keys);
                        foreach (var key in Keys) {
                            isSteppedFlagDictionary[key] = true;
                        }
                        changeAllSteppedFlagsTrue = false;
                    }
                    firstGetIsSteppedFlagInOneFixedUpdate = false; // こいつはlock要らないがここしか書けなそう
                    return isSteppedFlagDictionary[o];
                }
            }
        }
        public bool SetIsSteppedFlagFalse(System.Object o) { // Flagが全てFalseになってからfixedUpdateAutoWaitをSetしないと更新されてないのにStepが実行される
            lock (phSceneForGetSetLock) {
                if (!isSteppedFlagDictionary.ContainsKey(o)) {
                    return false;
                }
                isSteppedFlagDictionary[o] = false;
                if (!isSteppedFlagDictionary.Values.Contains(true)) { // 更新されていないものがまだある
                    //fixedUpdateAutoWait.Set();
                    allSteppedFlagsFalse = true;
                }
                return true;
            }
        }

        public override bool GetDesc(CsObject desc) {
            char ret = (char)0; // <!!> これいいのか？
            ObjectIf objectIf = this as ObjectIf;
            lock (phSceneForGetSetLock) {
                if (_thisArray[sceneForGet] != IntPtr.Zero) { // sceneForGet以外作られてない可能性あり
                    //Console.WriteLine("GetDesc(override) _thisArrayClassName " + objectIf.GetIfInfo().ClassName());
                    ret = SprExport.Spr_ObjectIf_GetDesc((IntPtr)_thisArray[sceneForGet], (IntPtr)desc);
                } else {
                    //Console.WriteLine("GetDesc(override) null _thisArrayClassName " + objectIf.GetIfInfo().ClassName());
                    ret = SprExport.Spr_ObjectIf_GetDesc((IntPtr)_this, (IntPtr)desc);
                }
            }
            return (ret == 0) ? false : true;
        }
        public override void SetDesc(CsObject desc) {
            // <!!> CDShapeは_thisだけしか作らないためnullチェックが必要、ここにもlockを掛ける必要があるがPHSceneIfにアクセスできない
            ObjectIf objectIf = this as ObjectIf;
            lock (phSceneForGetSetLock) {
                if (isStepping) {
                    //Console.WriteLine("SetDesc(override)" + " isStepping " + objectIf.GetIfInfo().ClassName());
                    AddWaitUntilNextStepCallback(() => {
                        //Console.WriteLine("SetDesc(override)" + " isStepping in Callback " + objectIf.GetIfInfo().ClassName());
                        if (_thisArray[sceneForStep] != IntPtr.Zero) { // こっちにCDShapeも入りえる
                            SprExport.Spr_ObjectIf_SetDesc((IntPtr)_thisArray[sceneForStep], (IntPtr)desc);
                        }
                        if (_thisArray[sceneForBuffer] != IntPtr.Zero) {
                            SprExport.Spr_ObjectIf_SetDesc((IntPtr)_thisArray[sceneForBuffer], (IntPtr)desc);
                        }
                    });
                    SprExport.Spr_ObjectIf_SetDesc((IntPtr)_thisArray[sceneForGet], (IntPtr)desc);
                } else {
                    //Console.WriteLine("SetDesc(override)" + " not isStepping " + objectIf.GetIfInfo().ClassName());
                    foreach (var _this in _thisArray) {
                        if (_this != IntPtr.Zero) {
                            SprExport.Spr_ObjectIf_SetDesc((IntPtr)_this, (IntPtr)desc);
                        }
                    }
                }
            }
        }
    }

    public partial class PHSpringIf : PHJointIf {
        public PHSpringIf(IntPtr ptr, IntPtr ptr1, IntPtr ptr2, bool flag = false) {
            _this = ptr; // <!!> SetNameのために
            _thisArray[0] = ptr;
            _thisArray[1] = ptr1;
            _thisArray[2] = ptr2;
            _flag = flag;
        }
        public void SetTargetVelocityAsync(Vec6d v) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        var newV = new Vec6d(v);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHSpringIf_SetTargetVelocity((IntPtr)_thisArray[phSceneIf.sceneForStep], (IntPtr)newV);
                            SprExport.Spr_PHSpringIf_SetTargetVelocity((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (IntPtr)newV);
                        });
                        SprExport.Spr_PHSpringIf_SetTargetVelocity((IntPtr)_thisArray[phSceneIf.sceneForGet], (IntPtr)newV);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHSpringIf_SetTargetVelocity((IntPtr)_this, (IntPtr)v);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHSpringIf_SetTargetVelocity((IntPtr)_this, (IntPtr)v);
            }
        }
        public void SetTargetOrientationAsync(Quaterniond q) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        var newQ = new Quaterniond(q);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHSpringIf_SetTargetOrientation((IntPtr)_thisArray[phSceneIf.sceneForStep], (IntPtr)newQ);
                            SprExport.Spr_PHSpringIf_SetTargetOrientation((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (IntPtr)newQ);
                        });
                        SprExport.Spr_PHSpringIf_SetTargetOrientation((IntPtr)_thisArray[phSceneIf.sceneForGet], (IntPtr)newQ);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHSpringIf_SetTargetOrientation((IntPtr)_this, (IntPtr)q);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHSpringIf_SetTargetOrientation((IntPtr)_this, (IntPtr)q);
            }
        }
        public void SetTargetPositionAsync(Vec3d p) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        var newP = new Vec3d(p);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHSpringIf_SetTargetPosition((IntPtr)_thisArray[phSceneIf.sceneForStep], (IntPtr)newP);
                            SprExport.Spr_PHSpringIf_SetTargetPosition((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (IntPtr)newP);
                        });
                        SprExport.Spr_PHSpringIf_SetTargetPosition((IntPtr)_thisArray[phSceneIf.sceneForGet], (IntPtr)newP);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHSpringIf_SetTargetPosition((IntPtr)_this, (IntPtr)p);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHSpringIf_SetTargetPosition((IntPtr)_this, (IntPtr)p);
            }
        }
    }
    public partial class PHBallJointIf : PHJointIf {
        public PHBallJointIf(IntPtr ptr, IntPtr ptr1, IntPtr ptr2, bool flag = false) {
            _thisArray[0] = ptr;
            _thisArray[1] = ptr1;
            _thisArray[2] = ptr2;
            _flag = flag;
        }
        // Thread処理のためのメソッド
        public void SetTargetVelocityAsync(Vec3d v) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        var newV = new Vec3d(v);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHBallJointIf_SetTargetVelocity((IntPtr)_thisArray[phSceneIf.sceneForStep], (IntPtr)newV);
                            SprExport.Spr_PHBallJointIf_SetTargetVelocity((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (IntPtr)newV);
                        });
                        SprExport.Spr_PHBallJointIf_SetTargetVelocity((IntPtr)_thisArray[phSceneIf.sceneForGet], (IntPtr)newV);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHBallJointIf_SetTargetVelocity((IntPtr)_this, (IntPtr)v);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHBallJointIf_SetTargetVelocity((IntPtr)_this, (IntPtr)v);
            }
        }
        public void SetTargetPositionAsync(Quaterniond q) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        //Console.WriteLine("SetTargetPositionAsync isStepping");
                        var newQ = new Quaterniond(q);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHBallJointIf_SetTargetPosition((IntPtr)_thisArray[phSceneIf.sceneForStep], (IntPtr)newQ);
                            SprExport.Spr_PHBallJointIf_SetTargetPosition((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (IntPtr)newQ);
                        });
                        SprExport.Spr_PHBallJointIf_SetTargetPosition((IntPtr)_thisArray[phSceneIf.sceneForGet], (IntPtr)newQ);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHBallJointIf_SetTargetPosition((IntPtr)_this, (IntPtr)q);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHBallJointIf_SetTargetPosition((IntPtr)_this, (IntPtr)q);
            }
        }
    }

    public partial class PHSolidIf : PHBodyIf {
        // Thread処理のためのメソッド
        public void SetPoseAsync(Posed p) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        var newP = new Posed(p);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHSolidIf_SetPose((IntPtr)_thisArray[phSceneIf.sceneForStep], (IntPtr)newP);
                            SprExport.Spr_PHSolidIf_SetPose((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (IntPtr)newP);
                        });
                        SprExport.Spr_PHSolidIf_SetPose((IntPtr)_thisArray[phSceneIf.sceneForGet], (IntPtr)newP);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHSolidIf_SetPose((IntPtr)_this, (IntPtr)p);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHSolidIf_SetPose((IntPtr)_this, (IntPtr)p);
            }
        }
        public void SetShapePoseAsync(int index, Posed p) {
            var phSceneIf = GetCSPHSceneIf();
            if (phSceneIf.threadMode) {
                lock (phSceneIf.phSceneForGetSetLock) {
                    if (phSceneIf.isStepping) {
                        var newP = new Posed(p);
                        phSceneIf.AddWaitUntilNextStepCallback(() => {
                            SprExport.Spr_PHBodyIf_SetShapePose((IntPtr)_thisArray[phSceneIf.sceneForStep], (int)index, (IntPtr)newP);
                            SprExport.Spr_PHBodyIf_SetShapePose((IntPtr)_thisArray[phSceneIf.sceneForBuffer], (int)index, (IntPtr)newP);
                        });
                        SprExport.Spr_PHBodyIf_SetShapePose((IntPtr)_thisArray[phSceneIf.sceneForGet], (int)index, (IntPtr)newP);
                    } else {
                        foreach (var _this in _thisArray) {
                            SprExport.Spr_PHBodyIf_SetShapePose((IntPtr)_this, (int)index, (IntPtr)p);
                        }
                    }
                }
            } else {
                SprExport.Spr_PHBodyIf_SetShapePose((IntPtr)_this, (int)index, (IntPtr)p);
            }
        }
    }

    public partial class PHBodyIf : SceneObjectIf {
        public Posed GetPoseAsync() {
            PHSceneIf phSceneIf = GetCSPHSceneIf();
            lock (phSceneIf.phSceneForGetSetLock) {
                phSceneIf.isFixedUpdating = true;
                IntPtr ptr = SprExport.Spr_PHBodyIf_GetPose(
                    (IntPtr)_thisArray[phSceneIf.sceneForGet]); // ここで取得されるPosedは複製
                return new Posed(ptr, true);
            }
        }
    }

}