#pragma once
#ifndef _SPOTLIGHT_H_
#define _SPOTLIGHT_H_

class SpotLight
{
public:
	SpotLight();
	~SpotLight();
private:
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Speclar;
	XMFLOAT3 Position;
	float Range;
	float Spot;
	XMFLOAT3 Att;
	float Pad;
};

#endif 