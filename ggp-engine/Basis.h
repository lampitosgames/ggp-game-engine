#ifndef GPP_BASIS_H
#define GPP_BASIS_H

#include <DirectXMath.h>

//TODO: Support scaling.

//Adapted from Godot's Basis class - https://github.com/godotengine/godot/blob/master/core/math/matrix3.h
class Basis {
	//Actual rotation/scale data for this basis vector matrix
	DirectX::XMFLOAT3X3 m;

	/*
		CONSTRUCTORS
	*/
	//Default constructor stores identity matrix
	Basis();
	//Constructor from 3 local axes
	Basis(const DirectX::XMFLOAT3 &_xAxis, const DirectX::XMFLOAT3 &_yAxis, const DirectX::XMFLOAT3 &_zAxis);
	//Constructor from all 9 float values
	Basis(float _xx, float _xy, float _xz, float _yx, float _yy, float _yz, float _zx, float _zy, float _zz);

	/*
		GET/SET of individual axes and the basis as a whole
	*/
	//Set every float in the basis individually
	void set(float _xx, float _xy, float _xz, float _yx, float _yy, float _yz, float _zx, float _zy, float _zz);
	//Set all three axes
	void set(const DirectX::XMFLOAT3 &_xAxis, const DirectX::XMFLOAT3 &_yAxis, const DirectX::XMFLOAT3 &_zAxis);
	//Get an individual axis
	DirectX::XMFLOAT3 getAxis(int _axisIndex);
	//Set an individual axis
	void setAxis(int _axisIndex, const DirectX::XMFLOAT3 &_axisVals);

	/*
		OVERLOAD OPERATORS
	*/
	//Overload access operator. Can't be used to set the individual axes
	DirectX::XMFLOAT3 &operator[](int _axisIndex) const;
};

#endif //GGP_BASIS_H