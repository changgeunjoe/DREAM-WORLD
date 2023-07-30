#include "stdafx.h"
#include "UILayer.h"
#include "Camera.h"
#include "GameobjectManager.h"
#include "GameFramework.h"
#include "Network/NetworkHelper.h"
#include "sound/GameSound.h"
extern NetworkHelper g_NetworkHelper;
extern CGameFramework gGameFramework;
extern GameSound g_sound;
using namespace std;

UILayer::UILayer(UINT nFrame, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue)
{
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_vWrappedRenderTargets.resize(nFrame);
	m_vd2dRenderTargets.resize(nFrame);
	m_vecTextBlocks.resize(TEXT_END);
	Initialize(pd3dDevice, pd3dCommandQueue);
}

UILayer::~UILayer()
{
	for (int j = 0; j < TEXT_TYPE::TEXT_END; ++j)
		for (auto& it : m_vecTextBlocks[j])
			delete (it);
}

void UILayer::Initialize(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue)
{
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = { };

#if defined(_DEBUG) || defined(DBG)
	d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ID3D11Device* pd3d11Device = NULL;
	ID3D12CommandQueue* ppd3dCommandQueues[] = { pd3dCommandQueue };
	::D3D11On12CreateDevice(pd3dDevice, d3d11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown**>(ppd3dCommandQueues), _countof(ppd3dCommandQueues), 0, (ID3D11Device**)&pd3d11Device, (ID3D11DeviceContext**)&m_pd3d11DeviceContext, nullptr);

	pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&m_pd3d11On12Device);
	pd3d11Device->Release();

#if defined(_DEBUG) || defined(DBG)
	ID3D12InfoQueue* pd3dInfoQueue;
	if (SUCCEEDED(pd3dDevice->QueryInterface(IID_PPV_ARGS(&pd3dInfoQueue))))
	{
		D3D12_MESSAGE_SEVERITY pd3dSeverities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO,
		};

		D3D12_MESSAGE_ID pd3dDenyIds[] =
		{
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
		};

		D3D12_INFO_QUEUE_FILTER d3dInforQueueFilter = { };
		d3dInforQueueFilter.DenyList.NumSeverities = _countof(pd3dSeverities);
		d3dInforQueueFilter.DenyList.pSeverityList = pd3dSeverities;
		d3dInforQueueFilter.DenyList.NumIDs = _countof(pd3dDenyIds);
		d3dInforQueueFilter.DenyList.pIDList = pd3dDenyIds;

		pd3dInfoQueue->PushStorageFilter(&d3dInforQueueFilter);
	}
	pd3dInfoQueue->Release();
#endif

	IDXGIDevice* pdxgiDevice = NULL;
	m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pdxgiDevice);

	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&m_pd2dFactory);
	HRESULT hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, (ID2D1Device2**)&m_pd2dDevice);
	m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, (ID2D1DeviceContext2**)&m_pd2dDeviceContext);

	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightPink), (ID2D1SolidColorBrush**)&m_pd2dTextBrush);

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pd2dWriteFactory);
	pdxgiDevice->Release();


}



void UILayer::UpdateLabels(const wstring& strUIText, int n)
{
	//if(n == 0)
	//    m_vTextBlocks[0] = { strUIText, D2D1::RectF(30.0f, 30.0f, m_fWidth, m_fHeight), m_pdwTextFormat };
	//else
	//    m_vTextBlocks[1] = { strUIText, D2D1::RectF(35.0f, 35.0f, m_fWidth, m_fHeight), m_pdwTextFormat };

}

void UILayer::Render(UINT nFrame)
{
	ID3D11Resource* ppResources[] = { m_vWrappedRenderTargets[nFrame] };

	m_pd2dDeviceContext->SetTarget(m_vd2dRenderTargets[nFrame]);

	m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

	m_pd2dDeviceContext->BeginDraw();

	for (int j = 0; j < TEXT_TYPE::TEXT_END; ++j)
	{
		for (auto i : m_vecTextBlocks[j])
			m_pd2dDeviceContext->DrawText(i->m_strText.c_str(), static_cast<UINT>(i->m_strText.length()), i->m_pdwFormat, i->m_d2dLayoutRect, m_pd2dTextBrush[int(i->m_eColor)]);
	}

	m_pd2dDeviceContext->EndDraw();

	m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
	m_pd3d11DeviceContext->Flush();
}

void UILayer::ReleaseResources()
{
	for (UINT i = 0; i < GetRenderTargetsCount(); i++)
	{
		ID3D11Resource* ppResources[] = { m_vWrappedRenderTargets[i] };
		m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
	}
	m_pd2dDeviceContext->SetTarget(nullptr);
	m_pd3d11DeviceContext->Flush();
	for (UINT i = 0; i < GetRenderTargetsCount(); i++)
	{
		m_vd2dRenderTargets[i]->Release();
		m_vWrappedRenderTargets[i]->Release();
	}
	for (UINT i = 0; i < TEXT_COLOR_END; i++)
		m_pd2dTextBrush[i]->Release();

	m_pd2dDeviceContext->Release();
	m_pdwTextFormat->Release();
	m_pd2dWriteFactory->Release();
	m_pd2dDevice->Release();
	m_pd2dFactory->Release();
	m_pd3d11DeviceContext->Release();
	m_pd3d11On12Device->Release();


}

void UILayer::Resize(ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{
	m_fWidth = static_cast<float>(nWidth);
	m_fHeight = static_cast<float>(nHeight);

	D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	for (UINT i = 0; i < GetRenderTargetsCount(); i++)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		m_pd3d11On12Device->CreateWrappedResource(ppd3dRenderTargets[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_vWrappedRenderTargets[i]));
		IDXGISurface* pdxgiSurface = NULL;
		m_vWrappedRenderTargets[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
		m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_vd2dRenderTargets[i]);
		pdxgiSurface->Release();
	}

	if (m_pd2dDeviceContext) m_pd2dDeviceContext->Release();
	m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dDeviceContext);
	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	/*if (m_pd2dTextBrush)
		for (UINT i = 0; i < TEXT_COLOR_END; i++)
			m_pd2dTextBrush[i]->Release();*/

	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pd2dTextBrush[TEXT_BLACK]);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), (ID2D1SolidColorBrush**)&m_pd2dTextBrush[TEXT_RED]);

	const float fFontSize = m_fHeight / 20.0f;
	const float fSmallFontSize = m_fHeight / 30.0f;




	ComPtr<IDWriteFontFile> fontFileReference;

	m_pd2dWriteFactory->CreateFontFileReference(L"Font/CookieRunBold.ttf", nullptr, &fontFileReference);

	ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
	m_pd2dWriteFactory->CreateFontSetBuilder(&fontSetBuilder);

	fontSetBuilder->AddFontFile(fontFileReference.Get());

	ComPtr<IDWriteFontSet> customFontSet;
	fontSetBuilder->CreateFontSet(&customFontSet);

	m_pd2dWriteFactory->CreateFontCollectionFromFontSet(
		customFontSet.Get()
		, &m_pdwFontCollection1
	);

	ComPtr<IDWriteFontFamily> fontFamily;
	ComPtr<IDWriteLocalizedStrings> localizedFontName;
	TCHAR c_styleFontName[65];

	m_pdwFontCollection1->GetFontFamily(0, &fontFamily);
	fontFamily->GetFamilyNames(&localizedFontName);
	localizedFontName->GetString(0, c_styleFontName, 65);
	m_pd2dWriteFactory->CreateTextFormat(c_styleFontName, m_pdwFontCollection1, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"ko", &m_pdwTextFormat);


	m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);


	m_pd2dWriteFactory->CreateTextFormat(c_styleFontName, m_pdwFontCollection1, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSmallFontSize, L"en-us", &m_pdwDamageFontFormat);

	m_pdwDamageFontFormat->SetTextAlignment(/*DWRITE_TEXT_ALIGNMENT_CENTER*/DWRITE_TEXT_ALIGNMENT_LEADING);
	m_pdwDamageFontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	//    m_pd2dWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSmallFontSize, L"en-us", &m_pdwTextFormat);
		/////////////
		//wstring str = L"";
		//CTextBlock* pTb = new CNPCTextBlock(m_pdwTextFormat, D2D1::RectF(0.f, 0.f, (float)FRAME_BUFFER_WIDTH, (float)FRAME_BUFFER_HEIGHT), str);
		//m_vecTextBlocks[TEXT_NPC].emplace_back(pTb);
}

void UILayer::Update(const float& fTimeElapsed, bool& binteraction, bool& bscreen)
{

	auto it = m_vecTextBlocks[TEXT_TYPE::TEXT_NPC].begin();
	while (it != m_vecTextBlocks[TEXT_TYPE::TEXT_NPC].end() && binteraction)
	{
		(*it)->Update(fTimeElapsed, binteraction, bscreen);

		if ((*it)->m_isDead)
		{
			delete (*it);
			it = m_vecTextBlocks[TEXT_NPC].erase(it);
		}
		else
			++it;
	}
	auto it2 = m_vecTextBlocks[TEXT_TYPE::TEXT_DAMAGE].begin();
	while (it2 != m_vecTextBlocks[TEXT_TYPE::TEXT_DAMAGE].end())
	{
		(*it2)->Update(fTimeElapsed, binteraction, bscreen);

		if ((*it2)->m_isDead)
		{
			delete (*it2);
			it2 = m_vecTextBlocks[TEXT_DAMAGE].erase(it2);
		}
		else
			++it2;
	}


}

void UILayer::AddDamageFont(XMFLOAT3 xmf3WorldPos, wstring strText)
{
	CTextBlock* pTb = new CDamageTextBlock(m_pdwDamageFontFormat, D2D1::RectF(0.f, 0.f, m_fWidth, m_fHeight), strText, xmf3WorldPos);
	pTb->m_eColor = TEXT_RED;
	m_vecTextBlocks[TEXT_DAMAGE].emplace_back(pTb);

}

void UILayer::AddTextFont(queue<wstring>& queueStr)
{
	if (!m_vecTextBlocks[TEXT_NPC].empty()) //NPC Text는 1개 씩
		m_vecTextBlocks[TEXT_NPC].front()->m_isDead = true;

	CTextBlock* pTb = new CNPCTextBlock(m_pdwTextFormat, D2D1::RectF(0.f, 0.f, m_fWidth, m_fHeight), queueStr);
	pTb->m_eColor = TEXT_BLACK;
	m_vecTextBlocks[TEXT_NPC].emplace_back(pTb);

}


XMFLOAT3 UILayer::WorldToScreen(XMFLOAT3& xmf3WorldPos)
{
	CCamera* pCamera= gGameFramework.GetCamera();
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Proj = pCamera->GetProjectionMatrix();

	// //카메라 거리 항상 같게-> 똑같음??
	// XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	// XMFLOAT3 xmf3ToCamera = Vector3::Subtract(xmf3CameraPos, xmf3WorldPos, true);
	// xmf3ToCamera = Vector3::ScalarProduct(xmf3ToCamera, 1.f);
	// xmf3CameraPos = Vector3::Add(xmf3WorldPos, xmf3ToCamera);

	// XMFLOAT4X4 xmf4x4ViewTemp = xmf4x4View;

	//// xmf4x4ViewTemp = Matrix4x4::LookAtLH(xmf3CameraPos, pCamera->m_xmf, m_xmf3Up);
	// xmf4x4ViewTemp._41 = -Vector3::DotProduct(xmf3CameraPos, pCamera->GetRightVector());
	// xmf4x4ViewTemp._42 = -Vector3::DotProduct(xmf3CameraPos, pCamera->GetUpVector());
	// xmf4x4ViewTemp._43 = -Vector3::DotProduct(xmf3CameraPos, pCamera->GetLookVector());


	XMFLOAT4X4 temp = Matrix4x4::Identity();
	XMVECTOR vecPos = XMLoadFloat3(&xmf3WorldPos);
	XMFLOAT3 xmf3ScreenPos;
	XMStoreFloat3(&xmf3ScreenPos, XMVector3Project(vecPos, 0.f, 0.f, (float)FRAME_BUFFER_WIDTH, (float)FRAME_BUFFER_HEIGHT, 0.f, 1.f,
		XMLoadFloat4x4(&xmf4x4Proj), XMLoadFloat4x4(&xmf4x4View), XMLoadFloat4x4(&temp)));

	float x = xmf3ScreenPos.x, y = xmf3ScreenPos.y, z = xmf3ScreenPos.z;
	//cout << x << "," << y <<  "," << z << endl;

	if (z <= 0.f || z >= 1.f) xmf3ScreenPos.x = -100.f;


	return xmf3ScreenPos;
}

////////////////////////////////////////////////////////////////////////

CTextBlock::CTextBlock()
{
}

CTextBlock::CTextBlock(IDWriteTextFormat* pdwFormat, D2D1_RECT_F& d2dLayoutRect, wstring& strText)
{
	m_pdwFormat = pdwFormat;
	m_d2dLayoutRect = d2dLayoutRect;
	m_strText = strText;
}

CTextBlock::~CTextBlock()
{
}

CDamageTextBlock::CDamageTextBlock(IDWriteTextFormat* pdwFormat, D2D1_RECT_F& d2dLayoutRect, wstring& strText, XMFLOAT3& xmf3WorldPos)
	:CTextBlock(pdwFormat, d2dLayoutRect, strText)
{
	m_xmf3WorldPos = xmf3WorldPos;
	m_fLifeTime = 0.45f;

	m_xmf3Velocity = { RandomValue(-0.4f, 0.4f), 0.3f, RandomValue(-0.4f, 0.4f) };
	// m_xmf3Velocity = Vector3::Normalize(m_xmf3Velocity);

	m_xmf3Accel = { 0.f, 1.f, 0.f };
}

CDamageTextBlock::~CDamageTextBlock()
{
}

void CDamageTextBlock::Update(const float& fTimeElapsed, bool& bInteraction, bool& bscreen)
{
	m_fLifeTime -= fTimeElapsed;
	if (m_fLifeTime < 0.f)
	{
		m_isDead = true;
		return;
	}

	//m_xmf3WorldPos.y += fTimeElapsed * 1.f;
	m_fTime += fTimeElapsed;
	float t = m_fTime, tt = t * t * 0.5f;

	m_xmf3WorldPos = Vector3::Add(m_xmf3WorldPos, Vector3::Add(Vector3::ScalarProduct(m_xmf3Accel, tt, false), Vector3::ScalarProduct(m_xmf3Velocity, t, false)));
	//m_xmf3WorldPos = Vector3::ScalarProduct(xmf3Velocity, t, false);


	//newPos = newPos + (t * a_Velocity) + (0.5 * u_Accel * tt);

	XMFLOAT3 xmf3ScreenPos = UILayer::WorldToScreen(m_xmf3WorldPos);
	m_d2dLayoutRect.left = xmf3ScreenPos.x;
	m_d2dLayoutRect.top = xmf3ScreenPos.y;
}


CNPCTextBlock::CNPCTextBlock(IDWriteTextFormat* pdwFormat, D2D1_RECT_F& d2dLayoutRect, queue<wstring>& queueStr) :CTextBlock()
{
	m_pdwFormat = pdwFormat;
	m_d2dLayoutRect = d2dLayoutRect;
	m_qTotalText = queueStr;

	m_d2dLayoutRect.left = FRAME_BUFFER_WIDTH * 0.25f;
	m_d2dLayoutRect.top = FRAME_BUFFER_HEIGHT * 0.85f;
}

CNPCTextBlock::~CNPCTextBlock()
{
}
#define CHARACTHER_DELAY 0.1f
#define SENTENCE_DELAY 1.5f
void CNPCTextBlock::Update(const float& fTimeElapsed, bool& bInteraction, bool& bscreen)
{

	m_fTime += fTimeElapsed;
	
	if (gGameFramework.GetScene()->GetObjectManager()->m_bSkipText) {//H누르면 강제 스킵 //아 텍스트는 남아있네 .. ㅠㅠ h키누르고 안없어지면 g키 한번 더 
		//눌러주세요 스킵은 된겁니다 잔상만 남아있으
		bscreen = false;
		bInteraction = false;
		//m_strText.clear();
		g_NetworkHelper.SendSkipNPCCommunicate();//소켓 전송
		gGameFramework.GetScene()->GetObjectManager()->m_bSkipText = false;
	}
	if (m_fTime > CHARACTHER_DELAY && !m_qTotalText.empty())
	{
		wstring curTotalStr = m_qTotalText.front();
		//m_strText.assign(m_strTotalText, 0, ++m_iIndex);
		if (!m_bInitSentences) {
			
			
			m_strText.append(curTotalStr, m_iIndex++, 1);
			if (curTotalStr[m_iIndex-1] != ' ') {
				g_sound.NoLoopPlay("UISound", 0.6f);
			}
		}
		if (m_iIndex > curTotalStr.size()) //한 문장 끝나면
		{
			bInteraction = false;
			//m_strText.erase();
			//m_strText.clear(); // 
			//m_isDead = true;
			//m_qTotalText.pop();

			m_iIndex = 0;
			m_bInitSentences = true;
			//m_isSentenceEnd = true;
		}
		if (bInteraction && m_bInitSentences) {//
			m_strText.clear();
			m_qTotalText.pop();
			m_bInitSentences = false;
		}
		if (m_qTotalText.empty()) {//npc대화 끝날때
			bscreen = false;
			bInteraction = false;
			if (gGameFramework.GetScene()->GetObjectManager()->m_bSendNPCPK) {
				cout << "소켓전송" << endl;
				g_NetworkHelper.SendSkipNPCCommunicate();//소켓 전송
				g_sound.Pause("LobbySound");//로비씬
				g_sound.Play("Stage1Sound", 0.42f);//로비씬
				
				gGameFramework.GetScene()->GetObjectManager()->m_bSendNPCPK = false;
				
							}
		}
		m_fTime = 0.f;

	}
}

CLobbyTextBlock::CLobbyTextBlock(IDWriteTextFormat* pdwFormat, D2D1_RECT_F& d2dLayoutRect, queue<wstring>& queueStr, XMFLOAT2 mf2LayoutRect)
{
	m_pdwFormat = pdwFormat;
	m_d2dLayoutRect = d2dLayoutRect;
	m_qTotalText = queueStr;
	//m_strTotalText = strText;
	m_d2dLayoutRect.left = FRAME_BUFFER_WIDTH * mf2LayoutRect.x;
	m_d2dLayoutRect.top = FRAME_BUFFER_HEIGHT * mf2LayoutRect.y;

}

CLobbyTextBlock::~CLobbyTextBlock()
{
}

void CLobbyTextBlock::Update(const float& fTimeElapsed, bool& bInteraction, bool& bscreen)
{
	//m_fTime += fTimeElapsed;
	//if (m_qTotalText.empty()) {
	//	bscreen = false;
	//	bInteraction = false;
	//}
	//if (m_fTime > CHARACTHER_DELAY && !m_qTotalText.empty())
	//{
	//	wstring curTotalStr = m_qTotalText.front();
	//	//m_strText.assign(m_strTotalText, 0, ++m_iIndex);
	//	if (!m_bInitSentences) {
	//		m_strText.append(curTotalStr, m_iIndex++, 1);
	//	}
	//	if (m_iIndex > curTotalStr.size()) //한 문장 끝나면
	//	{
	//		bInteraction = false;
	//		//m_strText.erase();
	//		//m_strText.clear(); // 
	//		//m_isDead = true;
	//		//m_qTotalText.pop();

	//		m_iIndex = 0;
	//		m_bInitSentences = true;
	//		//m_isSentenceEnd = true;
	//	}
	//
}
