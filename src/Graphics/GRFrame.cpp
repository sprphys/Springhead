#include "Graphics.h"
#include "GRFrame.h"

namespace Spr{;
OBJECTIMPABST(GRVisual, SceneObject);
IF_IMP(GRVisual, SceneObject);

OBJECTIMP(GRFrame, GRVisual);
IF_IMP(GRFrame, GRVisual);

void GRFrame::Render(GRRenderIf* r){
	r->PushModelMatrix();
	r->MultModelMatrix(transform);
	for(GRVisuals::iterator it = children.begin(); it != children.end(); ++it){
		(*it)->Render(r);
	}
	for(GRVisuals::reverse_iterator it = children.rbegin(); it != children.rend(); ++it){
		(*it)->Rendered(r);
	}
}
void GRFrame::Rendered(GRRenderIf* r){
	r->PopModelMatrix();
}
void GRFrame::SetParent(GRFrameIf* fr){
	if(parent == fr) return;
	if(parent){
		parent->DelChildObject((GRFrameIf*)this);
		parent=NULL;
	}
	if (fr){
		parent = OCAST(GRFrame, fr);
		fr->AddChildObject(fr);
	}
}
bool GRFrame::AddChildObject(ObjectIf* v){
	if (v->GetIfInfo()->Inherit(GRVisualIf::GetIfInfoStatic())){
		children.push_back(OCAST(GRVisual, v));
		return true;
	}
	return false;
}
bool GRFrame::DelChildObject(ObjectIf* v){
	for(GRVisuals::iterator it = children.begin(); it != children.end(); ++it){
		if (*it == OCAST(Object, v)){
			children.erase(it);
			return true;
		}
	}
	return false;
}
size_t GRFrame::NChildObject() const {
	return children.size();
}
ObjectIf* GRFrame::CreateObject(const IfInfo* info, const void* desc){
//TODO	if()
	return NULL;
}
ObjectIf* GRFrame::GetChildObject(size_t pos){
	return (Object*)children[pos];
}

}
