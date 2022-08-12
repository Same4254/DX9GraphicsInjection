#pragma once

#include "pch.h"
#include "dx.hpp"

struct Vertex {
	float x, y, z;
	D3DCOLOR color;
};

class Drawing {
private:
	static PDIRECT3DVERTEXBUFFER9 cubeVertexBuffer;
	static LPDIRECT3DVERTEXDECLARATION9 vertexLayoutDeclaration;

	static IDirect3DVertexShader9* vertexShader;
	static IDirect3DPixelShader9* pixelShader;
	static LPD3DXCONSTANTTABLE vertexShaderCompileConstTable;

	static void CreateShaders(LPDIRECT3DDEVICE9 device);
	static void DrawFilledRect(LPDIRECT3DDEVICE9 device, int x, int y, int width, int height, D3DCOLOR color);

	static void SetShaderMVPMatrix(LPDIRECT3DDEVICE9 device, D3DXMATRIX mvpMatrix);

public:
	static void Init(LPDIRECT3DDEVICE9 device);
	static void Draw(LPDIRECT3DDEVICE9 device, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix);
};