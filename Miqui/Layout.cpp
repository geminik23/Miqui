#pragma once
#include "pch.h"
#include "BasicType.h"

using namespace Miqui;


std::list<Layout*> Layout::layout_container = std::list<Layout*>();

void Layout::LayoutUpdate()
{
	for (auto&&c : Layout::layout_container)
		c->requestUpdate();
}


Layout::Layout()
{
	layout_container.push_back(this);
}

Layout::~Layout()
{
	layout_container.remove(this);
}

D2D1_POINT_2F Layout::queryPosition(IControl& control)
{
	return{ control.positionX() + control.translationX() + control.leftMargin() , control.positionY() + control.translationY() + control.topMargin() };
}


bool Layout::isInRegion(const D2D1_POINT_2F& point, IControl& control)
{
	auto p = this->queryPosition(control);
	D2D1_RECT_F rect{ p.x, p.y, control.width(), control.height() };
	rect.right += rect.left - 1;
	rect.bottom += rect.top - 1;

	return Miqui::isInRegion(point, rect);
}



D2D1::Matrix3x2F Layout::getTransformation(IControl& control)
{
	auto position = this->queryPosition(control);
	return
		D2D1::Matrix3x2F::Rotation(control.rotationZ(), D2D1_POINT_2F{ control.pivotX(), control.pivotY() })
		* D2D1::Matrix3x2F::Scale(D2D1_SIZE_F{ control.scaleX(), control.scaleY() } /*, scale_pivot*/)
		* D2D1::Matrix3x2F::Translation(position.x, position.y);
}


D2D1_SIZE_F Layout::getPossibleSize(IControl * control)
{
	D2D1_SIZE_F s{ control->preferredWidth(), control->preferredHeight() };

	if (control->isMinWidthSet()) s.width = (s.width < control->minWidth()) ? control->minWidth() : s.width;
	if (control->isMaxWidthSet()) s.width = (s.width > control->maxWidth()) ? control->maxWidth() : s.width;

	if (control->isMinHeightSet()) s.height = (s.height < control->minHeight()) ? control->minHeight() : s.height;
	if (control->isMaxHeightSet()) s.height = (s.height > control->maxHeight()) ? control->maxHeight() : s.height;

	return s;
}