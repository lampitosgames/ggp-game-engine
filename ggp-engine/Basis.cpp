#include "stdafx.h"

#include "Basis.h"

using namespace DirectX;

Basis::Basis() {
	m(0, 0) = 1;
	m(0, 1) = 0;
	m(0, 2) = 0;
	m(1, 0) = 0;
	m(1, 1) = 1;
	m(1, 2) = 0;
	m(2, 0) = 0;
	m(2, 1) = 0;
	m(2, 2) = 1;
}

Basis::Basis(const XMFLOAT3& _xAxis, const XMFLOAT3& _yAxis, const XMFLOAT3& _zAxis) {
	m(0, 0) = _xAxis.x;
	m(0, 1) = _xAxis.y;
	m(0, 2) = _xAxis.z;
	m(1, 0) = _yAxis.x;
	m(1, 1) = _yAxis.y;
	m(1, 2) = _yAxis.z;
	m(2, 0) = _zAxis.x;
	m(2, 1) = _zAxis.y;
	m(2, 2) = _zAxis.z;
}

void Basis::set(float _xx, float _xy, float _xz, float _yx, float _yy, float _yz, float _zx, float _zy, float _zz) {
	m(0, 0) = _xx;
	m(0, 1) = _xy;
	m(0, 2) = _xz;
	m(1, 0) = _yx;
	m(1, 1) = _yy;
	m(1, 2) = _yz;
	m(2, 0) = _zx;
	m(2, 1) = _zy;
	m(2, 2) = _zz;
}

void Basis::set(const XMFLOAT3 & _xAxis, const XMFLOAT3 & _yAxis, const XMFLOAT3 & _zAxis) {
	setAxis(0, _xAxis);
	setAxis(1, _yAxis);
	setAxis(2, _zAxis);
}

XMFLOAT3 Basis::getAxis(int _axisIndex) {
	//Get the specified axis vector for this local space
	return XMFLOAT3(m(_axisIndex, 0), m(_axisIndex, 1), m(_axisIndex, 2));
}

void Basis::setAxis(int _axisIndex, const DirectX::XMFLOAT3 & _axisVals) {
	m(_axisIndex, 0) = _axisVals.x;
	m(_axisIndex, 1) = _axisVals.y;
	m(_axisIndex, 2) = _axisVals.y;
}

XMFLOAT3 & Basis::operator[](int _axisIndex) const {
	//Get the specified axis vector for this local space
	return XMFLOAT3();//XMFLOAT3(m(_axisIndex, 0), m(_axisIndex, 1), m(_axisIndex, 2));
}
