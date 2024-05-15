#pragma once
class AstarNode
{
private:
	int m_nodeIdx = -1;
	float m_cost = 0.0f;
	float m_dis = 0.0f;
	float m_res = 0.0f;
	int m_parentNodeIdx = -1;

public:
	AstarNode() {}
	AstarNode(int nodeIdx, float cost, float dis, float res, int parentNodeIdx) : m_nodeIdx(nodeIdx), m_cost(cost), m_dis(dis), m_res(res), m_parentNodeIdx(parentNodeIdx) {}
	AstarNode(AstarNode& other)
	{
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
	}
	AstarNode(AstarNode&& other)
	{
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
	}
	~AstarNode() {}
public:
	void RefreshNodeData(int nodeIdx, float cost, float dis, float res, int parentNodeIdx)
	{
		m_nodeIdx = nodeIdx;
		m_cost = cost;
		m_dis = dis;
		m_res = res;
		m_parentNodeIdx = parentNodeIdx;
	}
	float GetResValue() { return m_res; }
	int GetIdx() { return m_nodeIdx; }
	int GetParentIdx() { return m_parentNodeIdx; }
	float GetDistance() { return m_dis; }

public:
	constexpr bool operator< (const AstarNode& other)const {
		return m_dis < other.m_dis;
	}
	AstarNode& operator= (const AstarNode& other) {
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
		return *this;
	}

	AstarNode& operator= (const AstarNode&& other) noexcept {
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
		return *this;
	}
};

