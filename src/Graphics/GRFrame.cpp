#include "Graphics.h"
#include "GRFrame.h"

namespace Spr{;
IF_OBJECT_IMP_ABST(GRVisual, NamedObject);

IF_OBJECT_IMP(GRFrame, GRVisual);
GRFrame::GRFrame(const GRFrameDesc& desc):GRFrameDesc(desc){
	parent = NULL;

}

void GRFrame::Render(GRRenderIf* r){
	r->PushModelMatrix();
	r->MultModelMatrix(transform);
	for(GRVisuals::iterator it = children.begin(); it != children.end(); ++it){
		(*it)->Render(r);
	}
	for(GRVisuals::reverse_iterator it = children.rbegin(); it != children.rend(); ++it){
		(*it)->Rendered(r);
	}
	r->PopModelMatrix();
}
void GRFrame::Rendered(GRRenderIf* r){
}
void GRFrame::SetNameManager(NameManager* m){
	assert(DCAST(GRScene, m));
	GRVisual::SetNameManager(m);
}
GRSceneIf* GRFrame::GetScene(){
	return DCAST(GRScene, GetNameManager());
}
void GRFrame::SetParent(GRFrameIf* fr){
	if(parent == fr) return;
	if(parent){
		parent->DelChildObject(this->GetIf());
		parent=NULL;
	}
	if (fr){
		parent = DCAST(GRFrame, fr);
		fr->AddChildObject(this->GetIf());
	}
}
bool GRFrame::AddChildObject(ObjectIf* o){
	GRVisual* v = DCAST(GRVisual, o);
	if (v){
		children.push_back(v);
		GRFrame* f = DCAST(GRFrame, v);
		if (f && f->parent != this){
			if (f->parent) f->parent->DelChildObject(f->GetIf());
			f->parent = this;
		}
		return true;
	}
	return false;
}
bool GRFrame::DelChildObject(ObjectIf* v){
	for(GRVisuals::iterator it = children.begin(); it != children.end(); ++it){
		if (*it == DCAST(Object, v)){
			children.erase(it);
			return true;
		}
	}
	return false;
}
size_t GRFrame::NChildObject() const {
	return children.size();
}
ObjectIf* GRFrame::GetChildObject(size_t pos){
	return (Object*)children[pos];
}

void GRFrame::Print(std::ostream& os) const {
	Object::PrintHeader(os);
	os.width(os.width()+2);
	os << transform;
	Object::PrintChildren(os);
	os.width(os.width()-2);
	Object::PrintFooter(os);
}

}
