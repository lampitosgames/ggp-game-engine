//Global definitions for cbuffers used in vertex shaders. Helpful for standardizing vshader inputs.

cbuffer externalData : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
	matrix worldInvTrans;
};