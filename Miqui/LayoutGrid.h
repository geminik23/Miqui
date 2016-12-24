#pragma once
#include "BasicType.h"


namespace Miqui
{

class GridLayout:public BaseLayout
{
public:
	virtual ~GridLayout() {}

	size_t ColumnCount() const { return m_columnCount; }
	void ColumnCount(size_t c) { m_columnCount = c; }
	void ResetColumnCount() { this->ColumnCount(1); }

	virtual void Layout(ControlContainer& container, std::list<IControl *>& controls) override
	{
		// 1. measuring
		for (auto&&c : controls)
		{
			c->MeasuringSize();
		}

		// 2. get container size and virtual size  -> each grid size
		auto cColumn = ColumnCount();
		size_t cRow = (controls.size() / cColumn) + ((controls.size() % cColumn) ? 1 : 0);

		float eachColumn = (container.Width() - (container.LeftPadding() + container.RightPadding())) / cColumn;
		float eachRow = (container.Height() - (container.TopPadding() + container.BottomPadding())) / cRow;

		float currentX = container.LeftPadding();
		float currentY = container.TopPadding();

		size_t ccount = 0;
		// 3. set size of each control
		for (auto&&c : controls)
		{
			switch (c->horizontalAlignment())
			{
			case HorizontalAlignmentType::Fill:
				c->Width(eachColumn - (c->LeftMargin() + c->RightMargin()));
				c->PositionX(currentX);
				break;
			case HorizontalAlignmentType::Left:
				c->PositionX(currentX);
				break;
			case HorizontalAlignmentType::Center:
				c->PositionX(currentX + (eachColumn - (c->LeftMargin() + c->RightMargin() + c->Width())) / 2);
				break;
			case HorizontalAlignmentType::Right:
				c->PositionX(currentX + eachColumn - (c->LeftMargin() + c->RightMargin() + c->Width()));
				break;
			}

			switch (c->verticalAlignment())
			{
			case VerticalAlignmentType::Fill:
				c->Height(eachRow - (c->TopMargin() + c->BottomMargin()));
				c->PositionY(currentY);
				break;
			case VerticalAlignmentType::Top:
				c->PositionY(currentY);
				break;
			case VerticalAlignmentType::Center:
				c->PositionY(currentY + (eachRow - (c->TopMargin() + c->BottomMargin() + c->Height())) / 2);
				break;
			case VerticalAlignmentType::Bottom:
				c->PositionY(currentY + eachRow - (c->TopMargin() + c->BottomMargin() + c->Height()));
				break;
			}


			ccount++;
			if (ccount>= cColumn)
			{
				ccount%= cColumn;
				currentY += eachRow;
				currentX = container.LeftPadding();
			} else
			{
				currentX += eachColumn;
			}

		}
		this->_arrangeComplete();
	}

	// Builder
	struct Builder
	{
		operator std::shared_ptr<BaseLayout>()
		{
			return obj;
		}

		std::shared_ptr<GridLayout> obj;
	};

	static Builder Create()
	{
		Builder b{ std::make_shared<GridLayout>() };
		return b;
	}

private:
	size_t m_columnCount{ 1 };
};



}