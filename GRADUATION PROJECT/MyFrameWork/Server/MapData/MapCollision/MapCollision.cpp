#include "stdafx.h"
//#include "MapCollision.h"
//
//MapCollision::MapCollision(XMFLOAT3& center, XMFLOAT3& extent, XMFLOAT4& quaternion, float& dotRes1, XMFLOAT3& normalVec1, float& dotRes2, XMFLOAT3& normalVec2)
//{
//	m_obb = BoundingOrientedBox(center, extent, quaternion);
//	normalVectorData[0] = std::make_pair(dotRes1, normalVec1);
//	normalVectorData[1] = std::make_pair(dotRes2, normalVec2);
//}
//
//std::tuple<XMFLOAT3, XMFLOAT3, float, float> MapCollision::CalSlidingVector(BoundingSphere& boundingSphere, XMFLOAT3& position, XMFLOAT3& moveDirectionVector)//�浹�� ���� �븻 ����, ���� �����̵� ���� ��ȯ, ��� ����, �����̵� ���� 
//{
//	XMFLOAT3 retSlidingVector;
//	XMFLOAT3 retnormalVector;
//	float retNormalVecDotProduct;
//	//float	 retIntersectDistacneNormalVector;
//	float	 retSlidingVecDotProduct;
//	//center->player vector
//	XMFLOAT3 center = m_obb.Center;
//	center.y = 0.0f;
//	/*XMFLOAT3 tempPosition = position;
//	tempPosition = Vector3::Add(position, Vector3::ScalarProduct(moveDirectionVector, -1.0f ));*/
//	XMFLOAT3 centerToPlayerVector = Vector3::Subtract(position, center);//����
//	centerToPlayerVector.y = 0.0f;
//	float centerToPlayerDisrtance = Vector3::Length(centerToPlayerVector);//�Ÿ�
//	centerToPlayerVector = Vector3::Normalize(centerToPlayerVector);
//	//float playerCollisionSphereIntersectDistance = 0.0f;
//	//�÷��̾ ��ġ�� ��Ž���� ����
//	float forwardDotResult = Vector3::DotProduct(centerToPlayerVector, normalVectorData[0].second);//��ü�� center�� �÷��̾�� normal���� cos��
//	if (std::abs(forwardDotResult) >= normalVectorData[0].first) {//�븻 ���Ͱ� forward�� �� 2�� �Ǵ�		
//		//set normal
//		if (forwardDotResult >= 0) retnormalVector = normalVectorData[0].second;
//		else retnormalVector = Vector3::ScalarProduct(normalVectorData[0].second, -1.0f, true);
//		//set sliding
//		float judgeSlidingVectorDotRes = Vector3::DotProduct(normalVectorData[1].second, moveDirectionVector);//�ݸ��� ��ü�� right ���Ϳ� ����
//		if (judgeSlidingVectorDotRes >= 0) retSlidingVector = normalVectorData[1].second;
//		else retSlidingVector = Vector3::ScalarProduct(normalVectorData[1].second, -1.0f, true);
//		//playerCollisionSphereIntersectDistance = centerToPlayerDisrtance * std::abs(forwardDotResult) - m_obb.Extents.z;
//		retNormalVecDotProduct = std::abs(Vector3::DotProduct(normalVectorData[0].second, moveDirectionVector));
//		retSlidingVecDotProduct = std::abs(judgeSlidingVectorDotRes);
//	}
//	else {
//		float rightDotResult = Vector3::DotProduct(centerToPlayerVector, normalVectorData[1].second);
//		//set normal
//		if (rightDotResult >= 0) retnormalVector = normalVectorData[1].second;
//		else retnormalVector = Vector3::ScalarProduct(normalVectorData[1].second, -1.0f, true);
//		//set sliding
//		float judgeSlidingVectorDotRes = Vector3::DotProduct(normalVectorData[0].second, moveDirectionVector);//�ݸ��� ��ü�� right ���Ϳ� ����
//		if (judgeSlidingVectorDotRes >= 0) retSlidingVector = normalVectorData[0].second;
//		else retSlidingVector = Vector3::ScalarProduct(normalVectorData[0].second, -1.0f, true);
//		//playerCollisionSphereIntersectDistance = centerToPlayerDisrtance * std::abs(rightDotResult) - m_obb.Extents.x;
//		retNormalVecDotProduct = std::abs(Vector3::DotProduct(normalVectorData[1].second, moveDirectionVector));
//		retSlidingVecDotProduct = std::abs(judgeSlidingVectorDotRes);
//	}
//	//retIntersectDistacneNormalVector = boundingSphere.Radius - playerCollisionSphereIntersectDistance;
//	//if (std::abs(retIntersectDistacneNormalVector) < DBL_EPSILON)retIntersectDistacneNormalVector = 0.0f;
//	return { retnormalVector, retSlidingVector, retNormalVecDotProduct, retSlidingVecDotProduct };
//}
