#include "pch.h"
#include "Drawing.hpp"
#include <iostream>

PDIRECT3DVERTEXBUFFER9 Drawing::cubeVertexBuffer;
LPDIRECT3DVERTEXDECLARATION9 Drawing::vertexLayoutDeclaration;

IDirect3DVertexShader9* Drawing::vertexShader;
IDirect3DPixelShader9* Drawing::pixelShader;
LPD3DXCONSTANTTABLE Drawing::vertexShaderCompileConstTable;

void Drawing::CreateShaders(LPDIRECT3DDEVICE9 device) {
	// An increadibly simple vertex and fragment shader to draw triangles with colors
	LPCSTR vertexShaderSource = R"(
        float4x4 WorldViewProj;

        struct VS_INPUT {
            float4 pos:POSITION;
            float4 c:COLOR;
        };

		struct VS_OUTPUT { 
			float4 pos:POSITION;
            float4 c:COLOR;
		};

		VS_OUTPUT main(VS_INPUT input) { 
			VS_OUTPUT Out; 
			Out.pos = mul(input.pos, WorldViewProj);
            Out.c = input.c;
			return Out;
		})";

	LPD3DXBUFFER vertexShaderCompiled, vertexShaderCompileErrors;

	HRESULT result = D3DXCompileShader(vertexShaderSource, strlen(vertexShaderSource), NULL, NULL, "main", D3DXGetVertexShaderProfile(device),
		D3DXSHADER_DEBUG, &vertexShaderCompiled, &vertexShaderCompileErrors, &vertexShaderCompileConstTable);

	if (result != D3D_OK) {
		std::cout << "Vertex Shader Compile Failed :(" << std::endl;

		LPVOID errPtr = vertexShaderCompileErrors->GetBufferPointer();
		DWORD len = vertexShaderCompileErrors->GetBufferSize();

		std::cout << (char*)errPtr << std::endl;
	}

	LPCSTR pixelShaderSource = R"(
		float4 main(float4 c:COLOR):COLOR { 
			return c;
		})";

	LPD3DXBUFFER pixelShaderCompiled, pixelShaderCompileErrors;
	LPD3DXCONSTANTTABLE pixelShaderCompileConstTable;

	result = D3DXCompileShader(pixelShaderSource, strlen(pixelShaderSource), NULL, NULL, "main", D3DXGetPixelShaderProfile(device),
		D3DXSHADER_DEBUG, &pixelShaderCompiled, &pixelShaderCompileErrors, &pixelShaderCompileConstTable);

	if (result != D3D_OK) {
		std::cout << "Pixel Shader Compile Failed :(" << std::endl;

		LPVOID errPtr = pixelShaderCompileErrors->GetBufferPointer();
		DWORD len = pixelShaderCompileErrors->GetBufferSize();

		std::cout << (char*)errPtr << std::endl;
	}

	result = device->CreateVertexShader((DWORD*)vertexShaderCompiled->GetBufferPointer(), &vertexShader);
	if (result != D3D_OK) {
		std::cout << "Vertex Shader Creation failed :(" << std::endl;
		std::cout << result << std::endl;
	}

	result = device->CreatePixelShader((DWORD*)pixelShaderCompiled->GetBufferPointer(), &pixelShader);
	if (result != D3D_OK) {
		std::cout << "Pixel Shader Creation failed :(" << std::endl;
		std::cout << result << std::endl;
	}
}

void Drawing::Init(LPDIRECT3DDEVICE9 device) {
	//Separate method because it's a lot of code
	CreateShaders(device);

	// Proof of concept rainbow cube!
	Vertex cubeVertices[] = {
		{ -1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },

		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{  1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 255) },

		{ -1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{ -1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 0) },

		{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{ -1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },

		{  1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		{  1.0f,  1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{ -1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 0) },

		{ -1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{  1.0f,  1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{ -1.0f,  1.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 255) },

		{  1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 255) },

		{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{  1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB(255, 0, 0) },

		{ -1.0f, 1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{  1.0f, 1.0f,  1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{ -1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 255) },

		{ -1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{  1.0f, 1.0f,  1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{  1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },

		{  1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },

		{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 255, 0) }


		//{ -1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		//{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },
		//{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0) },

		//{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		//{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },
		//{  1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 0) },

		//{ -1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		//{ -1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 255) },
		//{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 255) },

		//{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(255, 255, 0) },
		//{ -1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 0) },
		//{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 0) },

		//{  1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 255) },
		//{  1.0f,  1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 255) },
		//{ -1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 255) },

		//{ -1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		//{  1.0f,  1.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 255) },
		//{ -1.0f,  1.0f, 1.0f, D3DCOLOR_XRGB(0, 255, 255) },

		//{  1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 0) },
		//{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 0) },
		//{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 0) },

		//{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(255, 255, 255) },
		//{  1.0f,  1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 255) },
		//{  1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB(255, 255, 255) },

		//{ -1.0f, 1.0f,  1.0f, D3DCOLOR_XRGB(127, 127, 255) },
		//{  1.0f, 1.0f,  1.0f, D3DCOLOR_XRGB(127, 127, 255) },
		//{ -1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(127, 127, 255) },

		//{ -1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(127, 0, 0) },
		//{  1.0f, 1.0f,  1.0f, D3DCOLOR_XRGB(127, 0, 0) },
		//{  1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(127, 0, 0) },

		//{  1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 127, 0) },
		//{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 127, 0) },
		//{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 127, 0) },

		//{ -1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 0, 127) },
		//{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 127) },
		//{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB(0, 0, 127) }
	};

	//Layout of the vertex memory
	D3DVERTEXELEMENT9 vertexElements[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	device->CreateVertexDeclaration(vertexElements, &vertexLayoutDeclaration);
	device->SetVertexDeclaration(vertexLayoutDeclaration);

	device->CreateVertexBuffer(sizeof(cubeVertices),
		0,
		0,
		D3DPOOL_MANAGED,
		&cubeVertexBuffer,
		NULL);

	VOID* pVoid;

	// lock v_buffer and load the vertices into it
	cubeVertexBuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, cubeVertices, sizeof(cubeVertices));
	cubeVertexBuffer->Unlock();
}

void Drawing::DrawFilledRect(LPDIRECT3DDEVICE9 device, int x, int y, int width, int height, D3DCOLOR color) {
	D3DRECT rect = { x, y, x + width, y + height };
	device->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void Drawing::SetShaderMVPMatrix(LPDIRECT3DDEVICE9 device, D3DXMATRIX mvpMatrix) {
	D3DXCONSTANTTABLE_DESC tblDesc;
	HRESULT hr = vertexShaderCompileConstTable->GetDesc(&tblDesc);
	if (!FAILED(hr)) {
		D3DXHANDLE handle = vertexShaderCompileConstTable->GetConstantByName(NULL, "WorldViewProj");
		vertexShaderCompileConstTable->SetMatrix(device, handle, &mvpMatrix);

		//Debug output to make sure it is finding the constant in the table
		//D3DXCONSTANT_DESC d;
		//UINT n = 1;
		//if (!FAILED(vertexShaderCompileConstTable->GetConstantDesc(handle, &d, &n))) {
		//	std::cout << "  '"
		//		<< d.Name
		//		<< "' Registers:[begin:"
		//		<< (int)d.RegisterIndex
		//		<< ", count:"
		//		<< (int)d.RegisterCount
		//		<< "]" << std::endl;
		//}
	}
}

void Drawing::Draw(LPDIRECT3DDEVICE9 device, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix) {
	//To be safe, generally try to preserve original settings on the device

	//Store the current shader to restore at the end because the skybox in wizard101 does not set the shader
	IDirect3DVertexShader9* tempVertexShader;
	IDirect3DPixelShader9* tempPixelShader;

	device->GetVertexShader(&tempVertexShader);
	device->GetPixelShader(&tempPixelShader);

	device->SetVertexShader(vertexShader);
	device->SetPixelShader(pixelShader);

	DWORD originalSrcBlend, originalDestBlend;

	device->GetRenderState(D3DRS_SRCBLEND, &originalSrcBlend);
	device->GetRenderState(D3DRS_DESTBLEND, &originalDestBlend);

	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	//Enables depth testing
	DWORD originalDepthTest;
	device->GetRenderState(D3DRS_ZENABLE, &originalDepthTest);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	LPDIRECT3DVERTEXDECLARATION9 originalVertexDeclaration;
	device->GetVertexDeclaration(&originalVertexDeclaration);
	device->SetVertexDeclaration(vertexLayoutDeclaration);

	//***** Actual Drawing *****//

	DrawFilledRect(device, 25, 25, 100, 100, D3DCOLOR_ARGB(255, 255, 255, 255));

	D3DXMATRIX translate, rotation, scale;

	D3DXMatrixIdentity(&translate);
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixIdentity(&scale);

	D3DXMatrixScaling(&scale, 30, 30, 30);
	//D3DXMatrixTranslation(&translate, -951, -317, -491);

	D3DXMatrixTranslation(&translate, 200, 0, 0);

	D3DXMATRIX m = scale * rotation * translate;
	D3DXMATRIX mvp = m * viewMatrix * projectionMatrix;

	SetShaderMVPMatrix(device, mvp);

	device->SetStreamSource(0, cubeVertexBuffer, 0, sizeof(Vertex));
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);

	//***** Restore values *****//
	device->SetVertexDeclaration(originalVertexDeclaration);

	device->SetRenderState(D3DRS_ZENABLE, originalDepthTest);
	device->SetRenderState(D3DRS_SRCBLEND, originalSrcBlend);
	device->SetRenderState(D3DRS_DESTBLEND, originalDestBlend);

	device->SetVertexShader(tempVertexShader);
	device->SetPixelShader(tempPixelShader);
}
