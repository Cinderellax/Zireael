#include "ProcessorSelect.h"

using namespace Zireael;

Processor* ProcessorSelect::next()
{
	int n = _processors.size();
	if (!n)
	{
		return nullptr;
	}
	int minCoProIdx = 0;
	size_t minCoCnt = _processors.front()->getCoCnt();
	switch (_strategy)
	{
    //就他妈的从各个processor中挑选一个co数量最小的运行？？？？
	case MIN_EVENT_FIRST:
		for (int i = 1; i < n ; ++i)
		{
			size_t coCnt = _processors[i]->getCoCnt();
			if (coCnt < minCoCnt)
			{
				minCoCnt = coCnt;
				minCoProIdx = i;
			}
		}
		_curPro = minCoProIdx;
		break;
    //轮流运行。。。。
	case ROUND_ROBIN:
	default:
		++_curPro;
		if (_curPro >= n)
		{
			_curPro = 0;
		}
		break;
	}
	return _processors[_curPro];
};