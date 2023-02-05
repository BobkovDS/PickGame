#include <winrt/base.h>
#include "Utilit3D.h"
#include <string>

using namespace DirectX;
using namespace std;

ID3D12Device* Utilit3D::m_device = nullptr;
ID3D12GraphicsCommandList* Utilit3D::m_cmdList = nullptr;
bool Utilit3D::m_initialized = false;

Utilit3D::Utilit3D()
{
}

Utilit3D::~Utilit3D()
{
}

void Utilit3D::initialize(ID3D12Device* iDevice, ID3D12GraphicsCommandList* iCmdList)
{
	assert(iDevice);
	assert(iCmdList);

	m_device = iDevice;
	m_cmdList = iCmdList;
	m_initialized = true;
	std::string sd;
}

winrt::com_ptr<ID3D12Resource> Utilit3D::createDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	winrt::com_ptr<ID3D12Resource>& uploadBuffer)
{
	winrt::com_ptr<ID3D12Resource> defaultBuffer;
	HRESULT res;

	//create the actual default buffer resource
	res = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.put()));

	HRESULT removeReason;
	if (res != S_OK)
	{
		removeReason= device->GetDeviceRemovedReason();
		_com_error err(removeReason);
		wstring errMsg = err.ErrorMessage();

	}
	assert(SUCCEEDED(res));

	// To copy CPU memory to Default buffer, we need to create an intermediate upload heap
	res = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.put()));

	assert(SUCCEEDED(res));

	//describe the data we want to copy into default buffer
	D3D12_SUBRESOURCE_DATA subResource = {};
	subResource.pData = initData;
	subResource.RowPitch = byteSize;
	subResource.SlicePitch = byteSize;

	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(cmdList, defaultBuffer.get(), uploadBuffer.get(), 0, 0, 1, &subResource);

	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ));

	return defaultBuffer;
};

winrt::com_ptr<ID3D12Resource> Utilit3D::createTextureWithData
(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 elementByteSize,
	UINT Width,
	UINT Height,
	DXGI_FORMAT textureFormat,
	winrt::com_ptr<ID3D12Resource>& uploadBuffer,
	D3D12_RESOURCE_FLAGS textureFlags
)
{
	winrt::com_ptr<ID3D12Resource> lTextureResource;

	UINT64 lTextureSize = Width * Height * elementByteSize;
	D3D12_RESOURCE_DESC textDesc = {};
	textDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textDesc.Alignment = 0;
	textDesc.Width = Width;
	textDesc.Height = Height;
	textDesc.DepthOrArraySize = 1;
	textDesc.MipLevels = 1;
	textDesc.Format = textureFormat;
	textDesc.SampleDesc.Count = 1;
	textDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textDesc.Flags = textureFlags;

	HRESULT res = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&textDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(lTextureResource.put()));

	assert(SUCCEEDED(res));

	res = m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(lTextureSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.put()));

	assert(SUCCEEDED(res));
	
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = Width * elementByteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch * Height;

	UpdateSubresources(m_cmdList,
		lTextureResource.get(),
		uploadBuffer.get(),
		0, 0, 1,
		&subResourceData);

	m_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		lTextureResource.get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	return lTextureResource;
};

winrt::com_ptr<ID3D12Resource> Utilit3D::createDefaultBuffer(
	const void* initData,
	UINT64 byteSize,
	winrt::com_ptr<ID3D12Resource>& uploadBuffer)
{
	assert(m_initialized); //should be initialized at first

	return Utilit3D::createDefaultBuffer(m_device, m_cmdList, initData, byteSize, uploadBuffer);
}

ID3DBlob* Utilit3D::compileShader(
	const wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const string& entrypoint,
	const string& target)
{
	UINT compileflag = 0;
#if defined(DEBUG ) || defined(_DEBUG)
	compileflag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = S_OK;

	ID3DBlob* byteCode;
	ID3D10Blob* errors;

	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileflag, 0,
		&byteCode,
		&errors);

	if (hr != S_OK)
	{
		_com_error err(hr);
		wstring errMsg = err.ErrorMessage() + std::wstring(L" ") + filename;
		//throw MyCommonRuntimeException(errMsg, L"Shader compiler");
	}

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	return byteCode;
};

ID3DBlob* Utilit3D::compileShader(
	const string& filename,
	const D3D_SHADER_MACRO* defines,
	const string& entrypoint,
	const string& target)
{
	wstring fileName(filename.begin(), filename.end());
	return Utilit3D::compileShader(fileName, defines, entrypoint, target);
};

bool Utilit3D::compileShader(
	const wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const string& entrypoint,
	const string& target,
	ID3D10Blob** ptrBlob)
{

	UINT compileflag = 0;
#if defined(DEBUG ) || defined(_DEBUG)
	compileflag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = S_OK;
		
	winrt::com_ptr<ID3D10Blob> errors;
	bool result = false;

	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, \
		entrypoint.c_str(), target.c_str(), compileflag, 0, ptrBlob, errors.put());

	if (hr != S_OK)
	{
		_com_error err(hr);
		wstring errMsg = err.ErrorMessage() + std::wstring(L" ") + filename;
		//throw MyCommonRuntimeException(errMsg, L"Shader compiler");
	}
	else
		result = true;

	if (errors.get() != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	return result;
};

bool Utilit3D::compileShader(
	const string& filename,
	const D3D_SHADER_MACRO* defines,
	const string& entrypoint,
	const string& target,
	ID3D10Blob** ptrBlob)
{
	wstring fileName(filename.begin(), filename.end());
	return Utilit3D::compileShader(fileName, defines, entrypoint, target, ptrBlob);
};


void Utilit3D::UploadDDSTexture(
	std::string textureFileName,
	winrt::com_ptr<ID3D12Resource> &textureResource,
	winrt::com_ptr<ID3D12Resource> &uploader)
{
	assert(m_initialized); //should be initialized at first

	std::wstring tmpFleName(textureFileName.begin(), textureFileName.end());
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresource;

	textureResource = nullptr;

	HRESULT hr = LoadDDSTextureFromFile(
		m_device,
		tmpFleName.c_str(), textureResource.put(),
		ddsData,
		subresource);

	if (hr != S_OK)
	{
		_com_error err(hr);
		wstring errMsg = err.ErrorMessage() + std::wstring(L" for ")
			+ std::wstring(textureFileName.begin(), textureFileName.end());
		//throw MyCommonRuntimeException(errMsg, L"UploadDDSTexture");
	}

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource.get(), 0,
		static_cast<UINT>(subresource.size()));

	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploader.put()));

	UpdateSubresources(m_cmdList, textureResource.get(), uploader.get(),
		0, 0, static_cast<UINT>(subresource.size()), subresource.data());

	m_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureResource.get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE| D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

UINT Utilit3D::CalcConstantBufferByteSize(UINT byteSize)
{	
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512

	return (byteSize + 255) & ~255;
};

// ======================================================  Non-static versions