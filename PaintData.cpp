#include "PaintData.h"

PaintData::PaintData()
{
}

PaintData::~PaintData()
{
}

bool PaintData::pushNode(PaintNode nd)
{
	this->vecNode.push_back(nd);
	return true;
}

bool PaintData::popNode(PaintNode nd)
{
	this->vecNode.pop_back();
	return true;
}

bool PaintData::addNode(PaintNode nd, int index,int num)
{
	if (index > this->vecNode.size()) {
		return false;
	}
	std::vector<PaintNode>::iterator it;
	for (int i = 0; i < index; i++) it++;
	this->vecNode.insert(it, num, nd);
	return true;
}

PaintNode PaintData::getNode(int index)
{
	return (this->vecNode)[index];
}

int PaintData::getSize()
{
	return this->vecNode.size();
}

void PaintData::clear()
{
	this->vecNode.clear();
}

PaintNode::PaintNode()
{
}

PaintNode::PaintNode(POINT b, POINT e,PAINT_TYPE t):ptBegin(b),ptEnd(e),ptType(t)
{
}

PaintNode::~PaintNode()
{
}
