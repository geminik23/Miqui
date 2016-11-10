#include "pch.h"
#include "BasicType.h"

using namespace Miqui;

void Miqui::IControl::measuringSize()
{
	D2D1_SIZE_F size{ this->width(), this->height() };
	if (isPreferredWidthSet()) size.width = this->preferredWidth();
	if (isPreferredHeightSet()) size.height = this->preferredHeight();

	if (isMinWidthSet() && size.width < this->minWidth()) { size.width = minWidth(); }
	if (isMaxWidthSet() && size.width > this->maxWidth()) { size.width = maxWidth(); }
	if (isMinHeightSet() && size.height < this->minHeight()) { size.height = minHeight(); }
	if (isMaxHeightSet() && size.height > this->maxHeight()) { size.height = maxHeight(); }
	this->setWidth(size.width);
	this->setHeight(size.height);
}

void IControl::requestLayout()
{
	Layout::LayoutUpdate();
}