/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Graphics.h"
#include "GRMesh.h"

namespace Spr{;
IF_OBJECT_IMP(GRMesh, GRVisual);

GRMesh::GRMesh(GRMeshDesc& desc):GRMeshDesc(desc){
	list = 0;
	render = NULL;
}

void GRMesh::CreateList(GRRenderIf* r){
	if (list) render->ReleaseList(list);
	list = 0;
	render = r;
	if (texCoords.size() && normals.size() && colors.size()){
		std::vector<GRVertexElement::VFT2fC4fN3fP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].n = normals[i];
			vtx[i].c = colors[i];
			vtx[i].t = texCoords[i];
		}
		render->SetVertexFormat(GRVertexElement::vfT2fC4fN3fP3f);
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else if (texCoords.size() && normals.size()){
		std::vector<GRVertexElement::VFT2fN3fP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].n = normals[i];
			vtx[i].t = texCoords[i];
		}
		render->SetVertexFormat(GRVertexElement::vfT2fN3fP3f);
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else if (texCoords.size() && colors.size()){
		std::vector<GRVertexElement::VFT2fC4bP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].c = 
				((unsigned char)(colors[i].x*255) << 24) |
				((unsigned char)(colors[i].y*255) << 16) |
				((unsigned char)(colors[i].z*255) << 8) |
				((unsigned char)(colors[i].w*255));
			vtx[i].t = texCoords[i];
		}
		render->SetVertexFormat(GRVertexElement::vfT2fC4bP3f);
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else if (normals.size() && colors.size()){
		std::vector<GRVertexElement::VFC4fN3fP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].n = normals[i];
			vtx[i].c = colors[i];
		}
		render->SetVertexFormat(GRVertexElement::vfC4fN3fP3f);
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else if (normals.size()){
		std::vector<GRVertexElement::VFN3fP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].n = normals[i];
		}
		render->SetVertexFormat(GRVertexElement::vfN3fP3f);
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else if (texCoords.size()){
		std::vector<GRVertexElement::VFT2fP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].t = texCoords[i];
		}
		render->SetVertexFormat(GRVertexElement::vfT2fP3f);
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else if (colors.size()){
		std::vector<GRVertexElement::VFC4bP3f> vtx;
		vtx.resize(positions.size());
		for(unsigned i=0; i<vtx.size(); ++i){
			vtx[i].p = positions[i];
			vtx[i].c = 
				((unsigned char)(colors[i].x*255) << 24) |
				((unsigned char)(colors[i].y*255) << 16) |
				((unsigned char)(colors[i].z*255) << 8) |
				((unsigned char)(colors[i].w*255));
		}
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*vtx.begin(), vtx.size());
	}else{
		render->SetVertexFormat(GRVertexElement::vfP3f);
		render->SetMaterial(GRMaterialDesc(Vec4f(0,0,1,1)));
		list = render->CreateIndexedList(GRRender::TRIANGLES, &*faces.begin(), 
			&*positions.begin(), positions.size());
	}
}
void GRMesh::Render(GRRenderIf* r){
	if (r!=render || !list) CreateList(r);
	render->DrawList(list);
}
void GRMesh::Rendered(GRRenderIf* r){
}

}
