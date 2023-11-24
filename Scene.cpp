//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include <iostream>

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = false;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 40.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(20.0f, -5.0f, 10.0f);

	m_pLights[3].m_bEnable = false;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(0.0f, 50.0f, 0.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.01f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	XMFLOAT3 xmf3Scale(100.0f, 2.0f, 100.0f);
	XMFLOAT4 xmf4Color(0.0f, 1.0f, 0.0f, 1.0f);

	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("terrain.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
	m_pTerrain->SetPosition(XMFLOAT3(0.0, 0.0, 0.0));
	m_pTerrain->SetObjectID(0);

	m_nGameObjects = 32;
	m_ppGameObjects = new CGameObject * [m_nGameObjects];
	//%// m_ppGameObjects[0]~m_ppGameObjects[29]�� ������ ��ü�Դϴ�.
	for (int i = 0; i < m_nGameObjects; i++)
	{
		m_ppGameObjects[i] = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/bullet.bin");
		m_ppGameObjects[i]->SetScale(7.5f,15.0f,15.0f);
		//%// objecID 0���� 1���� ���� �ͷ��ΰ� �÷��̾�� �Ҵ�Ǿ��ֽ��ϴ� -> Shaders.hlsl���� Ȯ�� ����
		//%// �� �������� ���� ObjectID�� Ȧ¦ ���ο� ���� �������ֱ� ������ 2�� ���ؼ� ������ �Ȱ��� ó���صξ����ϴ�.
		m_ppGameObjects[i]->SetObjectID(i+2);

		//%// i�� ���� ¦���� type�� false��, Ȧ���϶� true�� �����߽��ϴ�.
		//%// m_ppGameObjects[i]->type�� false -> �ӵ� ���� / true -> �Ѿ� Ŀ���� ������ �Դϴ�.
		//%// �ӵ� ���� �������� ������
		//%// �Ѿ� ũ�� Ŀ���� �������� �Ķ����Դϴ�
		if (i % 2 == 0)
			m_ppGameObjects[i]->type = false;
		else
			m_ppGameObjects[i]->type = true;

		//%// ���� ���ϰ� ���̰� �Ϸ��� Rimpower�� �����ص׽��ϴ�
		m_ppGameObjects[i]->SetRimpower(0.1); 
		
		//%// ������ ��ġ�� �ѷ������� ���� �����߽��ϴ�. ���� �� ũ�Ⱑ �������� �ٽ� ������ �����Դϴ�
		XMFLOAT3 xmf3SponPosition = XMFLOAT3(0.0f + rand() % 2500, 0.0f, 0.0f + rand() % 2500);
		m_ppGameObjects[i]->SetPosition(xmf3SponPosition.x, 0.0, xmf3SponPosition.z);
	}

	//%// ���� �׸��� �κ��Դϴ�. �÷��̾�� ���������� SetPosition �Լ��� ��ġ�� �����մϴ�.
	CGameObject* pEnemy1 = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/tank.bin");
	pEnemy1->SetScale(1.0f, 1.0f, 1.0f);
	pEnemy1->SetPosition(1000.0f, 0.0f, 300.0f);
	pEnemy1->SetContext(m_pTerrain);
	m_ppGameObjects[30] = pEnemy1;
	m_ppGameObjects[30]->SetObjectID(1);
	m_ppGameObjects[30]->SetRimpower(5);
	m_ppGameObjects[30]->Enemy[0] = pEnemy1;

	CGameObject* pEnemy2 = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/tank.bin");
	pEnemy2->SetScale(1.0f, 1.0f, 1.0f);
	pEnemy2->SetPosition(800.0f, 0.0f, 300.0f);
	pEnemy2->SetContext(m_pTerrain);
	m_ppGameObjects[31] = pEnemy2;
	m_ppGameObjects[31]->SetObjectID(1);
	m_ppGameObjects[31]->SetRimpower(5);
	m_ppGameObjects[31]->Enemy[1] = pEnemy2;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);	
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
	if (m_pTerrain) delete m_pTerrain;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 34;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{

		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

XMFLOAT2 CScene::CheckIntersectRect(XMFLOAT2 OtherLT, XMFLOAT2 OtherRB, XMFLOAT2 ObjectLT, XMFLOAT2 ObjectRB)
{
	if (ObjectLT.x < OtherRB.x && ObjectRB.x > OtherLT.x && ObjectLT.y < OtherRB.y && ObjectRB.y > OtherLT.y) {
		XMFLOAT4 xmf4IntersectRect;

		xmf4IntersectRect.x = max(ObjectLT.x, OtherLT.x);
		xmf4IntersectRect.y = max(ObjectLT.y, OtherLT.y);
		xmf4IntersectRect.z = min(ObjectRB.x, OtherRB.x);
		xmf4IntersectRect.w = min(ObjectRB.y, OtherRB.y);

		XMFLOAT2 xmf2LenXY = XMFLOAT2(fabs(xmf4IntersectRect.z - xmf4IntersectRect.x), fabs(xmf4IntersectRect.y - xmf4IntersectRect.w));
		return xmf2LenXY;
	}
	return XMFLOAT2();
}

bool CScene::TCheckIntersectRect(XMFLOAT2 OtherLT, XMFLOAT2 OtherRB, XMFLOAT2 ObjectLT, XMFLOAT2 ObjectRB)
{
	if (ObjectLT.x < OtherRB.x && ObjectRB.x > OtherLT.x && ObjectLT.y < OtherRB.y && ObjectRB.y > OtherLT.y) {
		XMFLOAT4 xmf4IntersectRect;

		xmf4IntersectRect.x = max(ObjectLT.x, OtherLT.x);
		xmf4IntersectRect.y = max(ObjectLT.y, OtherLT.y);
		xmf4IntersectRect.z = min(ObjectRB.x, OtherRB.x);
		xmf4IntersectRect.w = min(ObjectRB.y, OtherRB.y);

		return true;
	}
	return false;
}

void CScene::CheckPlayerByItem() {
	//%// ��ü�� ������ �ѵ�üũ �˻���
	for (int i = 0; i < 30; ++i)
	{
		XMFLOAT3 e_pos = m_ppGameObjects[i]->GetPosition();
		XMFLOAT3 p_pos = m_pPlayer->GetPosition();

		XMFLOAT2 OtherLT = XMFLOAT2(e_pos.x - 20, e_pos.z - 30);
		XMFLOAT2 OtherRB = XMFLOAT2(e_pos.x + 20, e_pos.z + 30);
		XMFLOAT2 ObjectLT = XMFLOAT2(p_pos.x - 20, p_pos.z - 30);
		XMFLOAT2 ObjectRB = XMFLOAT2(p_pos.x + 20, p_pos.z + 30);

		XMFLOAT2 LenXY = CheckIntersectRect(OtherLT, OtherRB, ObjectLT, ObjectRB);
		if (LenXY.x || LenXY.y)
		{
			if (m_ppGameObjects[i]->isCollision == false) {
				if (LenXY.x < LenXY.y) {
					if (p_pos.x > m_ppGameObjects[i]->GetPosition().x)
						p_pos.x += LenXY.x;
					else
						p_pos.x -= LenXY.x;
				}
				else {
					if (p_pos.z > m_ppGameObjects[i]->GetPosition().z)
						p_pos.z += LenXY.y;
					else
						p_pos.z -= LenXY.y;
				}
			}
			m_ppGameObjects[i]->isCollision = true;
			
			
		}
		m_pPlayer->SetPosition(p_pos);
	}
}

void CScene::CheckPlayerByEnemy() {
	//%// ��ü�� �� �ѵ�üũ �˻���
	for (int i = 30; i < 32; ++i)
	{
		XMFLOAT3 e_pos = m_ppGameObjects[i]->GetPosition();
		XMFLOAT3 p_pos = m_pPlayer->GetPosition();

		XMFLOAT2 OtherLT = XMFLOAT2(e_pos.x - 20, e_pos.z - 30);
		XMFLOAT2 OtherRB = XMFLOAT2(e_pos.x + 20, e_pos.z + 30);
		XMFLOAT2 ObjectLT = XMFLOAT2(p_pos.x - 20, p_pos.z - 30);
		XMFLOAT2 ObjectRB = XMFLOAT2(p_pos.x + 20, p_pos.z + 30);

		XMFLOAT2 LenXY = CheckIntersectRect(OtherLT, OtherRB, ObjectLT, ObjectRB);
		if (LenXY.x || LenXY.y)
		{
			if (m_ppGameObjects[i]->isCollision == false) {
				if (LenXY.x < LenXY.y) {
					if (p_pos.x > m_ppGameObjects[i]->GetPosition().x)
						p_pos.x += LenXY.x;
					else
						p_pos.x -= LenXY.x;
				}
				else {
					if (p_pos.z > m_ppGameObjects[i]->GetPosition().z)
						p_pos.z += LenXY.y;
					else
						p_pos.z -= LenXY.y;
				}
			}


		}
		m_pPlayer->SetPosition(p_pos);
	}
}

void CScene::CheckEnemyByBullet(float time) {
	//%// ���� ��� ����
	CGameObject** ppBullets = ((CMyTankPlayer*)m_pPlayer)->m_ppBullets;
	for (int i = 0; i < Enemy_NUM; ++i)
	{
		for (int j = 0; j < BULLETS; ++j) {
			XMFLOAT3 e_pos = m_ppGameObjects[i]->GetPosition();
			XMFLOAT3 b_pos = ppBullets[j]->GetPosition();

			XMFLOAT2 OtherLT = XMFLOAT2(e_pos.x - 10, e_pos.z - 10);
			XMFLOAT2 OtherRB = XMFLOAT2(e_pos.x + 10, e_pos.z + 10);
			XMFLOAT2 ObjectLT = XMFLOAT2(b_pos.x - 10, b_pos.z - 5);
			XMFLOAT2 ObjectRB = XMFLOAT2(b_pos.x + 10, b_pos.z + 5);

			if (TCheckIntersectRect(OtherLT, OtherRB, ObjectLT, ObjectRB)) {
				m_ppGameObjects[i]->isCollision = true;
				ppBullets[j]->SetPosition(99999.0,88888.0,88888.0);
			}	
		}
		
	}
}

// �÷��̾�� ������ �浹 üũ
void CScene::IsCollision(float time) {
	//%// �����۰� �÷��̾� �浹�� Ȯ���մϴ�
	static float fTimeElapsed = 0.0f;  

	for (int i = 0; i < m_nGameObjects; ++i) {
		if (m_ppGameObjects[i]->isCollision) {
			if (m_ppGameObjects[i]->type) {
				// �Ѿ� ũ��
				m_pPlayer->m_BulletSizeX += 0.05f;
				m_pPlayer->m_BulletSizeY += 0.05f;
				m_pPlayer->m_BulletSizeZ += 0.05f;
				m_ppGameObjects[i]->isCollision = false;
				m_ppGameObjects[i]->draw = false;
				//m_ppGameObjects[i] = NULL;
			}
			else {
				m_pPlayer->m_vel += 10.0;
				m_ppGameObjects[i]->draw = false;
			}
		}
		
	}
}

float getdistance(XMFLOAT3& a, XMFLOAT3& b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.z - b.z, 2));
}

void CScene::OnPlayerUpdateCallback(float fTimeElapsed)
{
	//%// �ͷ��� ��ġ�� �°� �� ��ü�� y�� ���� �Լ�
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	for (int i = 0; i < m_nGameObjects; ++i)
	{
		if (m_ppGameObjects[i]->draw)
		{
			XMFLOAT3 xmf3ObjectPosition = m_ppGameObjects[i]->GetPosition();
			float fHeight = pTerrain->GetHeight(xmf3ObjectPosition.x, xmf3ObjectPosition.z) + 8.0f;

			if (xmf3ObjectPosition.y < fHeight)
			{
				xmf3ObjectPosition.y = fHeight;
				m_ppGameObjects[i]->SetPosition(xmf3ObjectPosition);
			}
		}
	}
}
void CScene::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	}
}



void CScene::CheckPlayerByObjectLen() {
	// �÷��̾�� ������Ʈ���� �Ÿ��� üũ�ؼ� �̵��� ���� bool �� ����
	//%// ���� ��� ����
	for (int i = 0; i < m_nGameObjects; ++i) {
		float dis = getdistance(m_ppGameObjects[i]->GetPosition(), ((CMyTankPlayer*)m_pPlayer)->GetPosition());

		if (dis <= 200.0f) {
			m_ppGameObjects[i]->followP = true;
		}
		else
			m_ppGameObjects[i]->followP = false;
	}

	
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nGameObjects; i++)
	{
		m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
	}

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
	CheckPlayerByItem();
	CheckPlayerByEnemy();
	//CheckEnemyByBullet(fTimeElapsed);
	//CheckPlayerByObjectLen();
	IsCollision(fTimeElapsed);

	OnPlayerUpdateCallback(fTimeElapsed);

}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nGameObjects; ++i)
	{
		if (m_ppGameObjects[i]->draw)
		{
			m_ppGameObjects[i]->Animate(m_fElapsedTime, NULL);
			m_ppGameObjects[i]->UpdateTransform(NULL);
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
}