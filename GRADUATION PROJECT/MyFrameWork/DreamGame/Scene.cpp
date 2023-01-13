#include "Scene.h"
CScene::CScene()
{

}

CScene::~CScene()
{
}

bool CScene::onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    return false;
}

bool CScene::onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    return false;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
    ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS

    RootSignature.Descriptorrange.resize(1);
    RootSignature.Descriptorrange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    RootSignature.Descriptorrange[0].NumDescriptors = 1;
    RootSignature.Descriptorrange[0].BaseShaderRegister = 0; //GameObject //b0
    RootSignature.Descriptorrange[0].RegisterSpace = 0;
    RootSignature.Descriptorrange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    //RootSignature.Descriptorrange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    //RootSignature.Descriptorrange[1].NumDescriptors = 1;
    //RootSignature.Descriptorrange[1].NumDescriptors = 1;
    //RootSignature.Descriptorrange[1].BaseShaderRegister = 1; //Camera //b1
    //RootSignature.Descriptorrange[1].RegisterSpace = 0;
    //RootSignature.Descriptorrange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    
//-------------------------------rootParameter----------------------------------------------------    
    RootSignature.RootParameter.resize(2);
    //GameObject(b0)Shaders.hlsl
    RootSignature.RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    RootSignature.RootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
    RootSignature.RootParameter[0].DescriptorTable.pDescriptorRanges = &(RootSignature.Descriptorrange[0]);
    RootSignature.RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    //Camera(b1) Shaders.hlsl
    RootSignature.RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootSignature.RootParameter[1].Descriptor.ShaderRegister = 1;
    RootSignature.RootParameter[1].Descriptor.RegisterSpace = 0;
    RootSignature.RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

#endif
    ::ZeroMemory(&RootSignature.RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
    RootSignature.RootSignatureDesc.NumParameters = RootSignature.RootParameter.size();
    RootSignature.RootSignatureDesc.pParameters = RootSignature.RootParameter.data();
    RootSignature.RootSignatureDesc.NumStaticSamplers = RootSignature.TextureSamplerDescs.size();
    RootSignature.RootSignatureDesc.pStaticSamplers = RootSignature.TextureSamplerDescs.data(); //texture sampler 아직 없음
    RootSignature.RootSignatureDesc.Flags = RootSignature.RootSignatureFlags;

    ID3DBlob* pd3dSignatureBlob = NULL;
    ID3DBlob* pd3dErrorBlob = NULL;
    D3D12SerializeRootSignature(&RootSignature.RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
    pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
    if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
    if (pd3dErrorBlob) pd3dErrorBlob->Release();

    return(pd3dGraphicsRootSignature);




    ////매개변수가 없는 루트 시그너쳐를 생성한다.
    //D3D12_ROOT_PARAMETER pd3dRootParameters[2];
    //pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    //pd3dRootParameters[0].Constants.Num32BitValues = 16;
    //pd3dRootParameters[0].Constants.ShaderRegister = 0;
    //pd3dRootParameters[0].Constants.RegisterSpace = 0;
    //pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    //pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    //pd3dRootParameters[1].Constants.Num32BitValues = 32;
    //pd3dRootParameters[1].Constants.ShaderRegister = 1;
    //pd3dRootParameters[1].Constants.RegisterSpace = 0;
    //pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    //D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
    //    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
    //D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
    //::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
    //d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
    //d3dRootSignatureDesc.pParameters = pd3dRootParameters;
    //d3dRootSignatureDesc.NumStaticSamplers = 0;
    //d3dRootSignatureDesc.pStaticSamplers = NULL;
    //d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
    //ID3DBlob* pd3dSignatureBlob = NULL;
    //ID3DBlob* pd3dErrorBlob = NULL;
    //::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
    //    &pd3dSignatureBlob, &pd3dErrorBlob);
    //pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
    //    pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
    //        **)&pd3dGraphicsRootSignature);
    //if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
    //if (pd3dErrorBlob) pd3dErrorBlob->Release();
    //return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
    return(m_pd3dGraphicsRootSignature);
}

//ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
//{
//    ID3D12RootSignature* pd3dGraphicRootSignature = NULL;
//
//    //매개변수가 없는 루트 시그너쳐를 생성한다.
//    D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
//    ::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
//    d3dRootSignatureDesc.NumParameters = 0;
//    d3dRootSignatureDesc.pParameters = NULL;
//    d3dRootSignatureDesc.NumStaticSamplers = 0;
//    d3dRootSignatureDesc.pStaticSamplers = NULL;
//    d3dRootSignatureDesc.Flags =
//        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//    ID3DBlob* pd3dSignatureBlob = NULL;
//    ID3DBlob* pd3dErrorBlob = NULL;
//
//    ::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
//        &pd3dSignatureBlob, &pd3dErrorBlob);
//    pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), 
//        __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicRootSignature);
//
//    if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
//    if (pd3dErrorBlob) pd3dErrorBlob->Release();
//
//    return(pd3dGraphicRootSignature);
//
////   //정점 셰이더와 픽셀셰이더를 생성한다.
////   ID3DBlob* pd3dVertexShaderBlob = NULL;
////   ID3DBlob* pd3dPixelShaderBlob = NULL;
////
////   UINT nCompileFlags = 0;
////#if defined(_DEBUG)
////   nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
////#endif
////
////   ID3DBlob* pd3dErrorBlob = NULL;
////
////   D3DCompileFromFile(L"Shaders.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_1", nCompileFlags, 0,
////      &pd3dVertexShaderBlob, &pd3dErrorBlob);
////   char* pErrorString = NULL;
////   if (pd3dErrorBlob) pErrorString = (char*)pd3dErrorBlob->GetBufferPointer();
////
////   D3DCompileFromFile(L"Shaders.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_1", nCompileFlags, 0,
////      &pd3dPixelShaderBlob, NULL);
////   //래스터라이저 상태를 설정한다.
////   D3D12_RASTERIZER_DESC d3dRasterizerDesc;
////   ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
////   d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
////   d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
////   d3dRasterizerDesc.FrontCounterClockwise = FALSE;
////   d3dRasterizerDesc.DepthBias = 0;
////   d3dRasterizerDesc.DepthBiasClamp = 0.0f;
////   d3dRasterizerDesc.MultisampleEnable = TRUE;
////   d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
////   d3dRasterizerDesc.ForcedSampleCount = 0;
////   d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
////
////   //블렌드 상태를 설정한다.
////   D3D12_BLEND_DESC d3dBlendDesc;
////   ::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
////   d3dBlendDesc.AlphaToCoverageEnable = FALSE;
////   d3dBlendDesc.IndependentBlendEnable = FALSE;
////   d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
////   d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
////   d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
////   d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
////   d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
////   d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
////   d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
////   d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
////   
////   //그래픽 파이프라인 상태를 설정한다.
////
////   D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
////   ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
////   d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
////   d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
////   d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
////   d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
////   d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
////   d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
////   d3dPipelineStateDesc.BlendState = d3dBlendDesc;
////   d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
////   d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
////   d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
////   d3dPipelineStateDesc.InputLayout.NumElements = 0;
////   d3dPipelineStateDesc.SampleMask = UINT_MAX;
////   d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
////   d3dPipelineStateDesc.NumRenderTargets = 1;
////   d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
////   d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
////   d3dPipelineStateDesc.SampleDesc.Count = 1;
////   d3dPipelineStateDesc.SampleDesc.Quality = 0;
////   pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, 
////      __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipeLineState);
////
////   if (pd3dVertexShaderBlob)pd3dVertexShaderBlob->Release();
////   if (pd3dPixelShaderBlob)pd3dPixelShaderBlob->Release();
////   
//
//}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

    m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

    ////가로x세로x깊이가 12x12x12인 정육면체 메쉬를 생성한다. 
    //CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
    //    12.0f, 12.0f, 12.0f);

    //m_nObjects = 1;
    //m_ppObjects = new CGameObject * [m_nObjects];

    //CRotatingObject* pRotatingObject = new CRotatingObject();
    //pRotatingObject->SetMesh(pCubeMesh);

    //CDiffusedShader* pShader = new CDiffusedShader();
    //pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
    //pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

    //pRotatingObject->SetShader(pShader);

    //m_ppObjects[0] = pRotatingObject;
    m_pObjectManager = new GameobjectManager();
    m_pObjectManager->BuildObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}


void CScene::ReleaseObjects()
{
    if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
    if (m_ppObjects)
    {
        for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
        delete[] m_ppObjects;
    }
}


bool CScene::ProcessInput()
{
    return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
    for (int j = 0; j < m_nObjects; j++)
    {
        m_ppObjects[j]->Animate(fTimeElapsed);
    }
}

//void CScene::PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//   //그래픽 루트 시그너쳐를 설정한다.
//   pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
//   //파이프라인 상태를 설정한다.
//   pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
//   //프리미티브 토폴로지(삼각형 리스트)를 설정한다.
//   pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}

void CScene::Render(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
    pCamera->SetViewportsAndScissorRects(pd3dCommandList);
    pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
    if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList);
    //씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다. 
    
    m_pObjectManager->Render(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
  /*  for (int j = 0; j < m_nObjects; j++)
    {
        if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
    }*/
}

void CScene::ReleaseUploadBuffers()
{
    if (m_ppObjects)
    {
        for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j])
            m_ppObjects[j]->ReleaseUploadBuffers();
    }
}

