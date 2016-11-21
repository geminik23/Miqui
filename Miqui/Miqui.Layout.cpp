#pragma once
#include "pch.h"
#include "BasicType.h"

using namespace Miqui;


std::list<BaseLayout*> BaseLayout::layout_container = std::list<BaseLayout*>();

void BaseLayout::LayoutUpdate()
{
	for (auto&&c : BaseLayout::layout_container)
		c->RequestUpdate();
}


BaseLayout::BaseLayout()
{
	layout_container.push_back(this);
}

BaseLayout::~BaseLayout()
{
	layout_container.remove(this);
}

D2D1_POINT_2F BaseLayout::QueryPosition(IControl& control)
{
	return{ control.PositionX() + control.TranslationX() + control.LeftMargin() , control.PositionY() + control.TranslationY() + control.TopMargin() };
}


bool BaseLayout::IsInRegion(const D2D1_POINT_2F& point, IControl& control)
{
	auto p = this->QueryPosition(control);
	D2D1_RECT_F rect{ p.x, p.y, control.Width(), control.Height() };
	rect.right += rect.left - 1;
	rect.bottom += rect.top - 1;

	return Miqui::isInRegion(point, rect);
}



D2D1::Matrix3x2F BaseLayout::Transformation(IControl& control)
{
	auto position = this->QueryPosition(control);
	return
		D2D1::Matrix3x2F::Rotation(control.RotationZ(), D2D1_POINT_2F{ control.PivotX(), control.PivotY() })
		* D2D1::Matrix3x2F::Scale(D2D1_SIZE_F{ control.ScaleX(), control.ScaleY() } /*, scale_pivot*/)
		* D2D1::Matrix3x2F::Translation(position.x, position.y);
}


D2D1_SIZE_F BaseLayout::GetPossibleSize(IControl * control)
{
	D2D1_SIZE_F s{ control->PreferredWidth(), control->PreferredHeight() };

	if (control->IsMaxWidthSet()) s.width = (s.width > control->MaxWidth()) ? control->MaxWidth() : s.width;

	if (control->IsMinHeightSet()) s.height = (s.height < control->MinHeight()) ? control->MinHeight() : s.height;
	if (control->IsMaxHeightSet()) s.height = (s.height > control->MaxHeight()) ? control->MaxHeight() : s.height;

	return s;
}