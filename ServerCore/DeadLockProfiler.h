#pragma once
#include <stack>
#include <map>
#include <vector>

// 그래프 자료구조를 통하여 데드락이 일어난 상황인지 확인하기 위한 클래스.
class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32>		_nameToId;
	unordered_map<int32, const char*>		_idToName;
	stack<int32>							_lockStack;
	map < int32, set<int32>>				_lockHistory;

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;		// 노드가 발견된 순서를 기록하는 배열
	int32			_discoveredCount = 0;	// 노드가 발견된 순서
	vector<bool>	_finished;				// DFS(i)가 종료되었는지 여부
	vector<int32>	_parent;				// 발견된 부모가 누구인지 추적하기 위함
};

