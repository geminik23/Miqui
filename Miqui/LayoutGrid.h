#pragma once
#include "BasicType.h"


namespace Miqui
{

class GridLayout:public Layout
{
public:
	virtual ~GridLayout() {}

	size_t columnCount() const { return m_columnCount; }
	void setColumnCount(size_t c) { m_columnCount = c; }
	void resetColumnCount() { this->setColumnCount(2); }

	virtual void layout(ControlContainer& container, std::list<IControl *>& controls) override
	{
		// 1. measuring
		for (auto&&c : controls)
		{
			c->measuringSize();
		}

		// 2. get container size and virtual size  -> each grid size
		auto cColumn = columnCount();
		size_t cRow = (controls.size() / cColumn) + ((controls.size() % cColumn) ? 1 : 0);

		float eachColumn = (container.width() - (container.leftPadding() + container.rightPadding())) / cColumn;
		float eachRow = (container.height() - (container.topPadding() + container.bottomPadding())) / cRow;

		float currentX = container.leftPadding();
		float currentY = container.topPadding();

		size_t ccount = 0;
		// 3. set size of each control
		for (auto&&c : controls)
		{
			switch (c->horizontalAlignment())
			{
			case HorizontalAlignment::Fill:
				c->setWidth(eachColumn - (c->leftMargin() + c->rightMargin()));
				c->setPositionX(currentX);
				break;
			case HorizontalAlignment::Left:
				c->setPositionX(currentX);
				break;
			case HorizontalAlignment::Center:
				c->setPositionX(currentX + (eachColumn - (c->leftMargin() + c->rightMargin() + c->width())) / 2);
				break;
			case HorizontalAlignment::Right:
				c->setPositionX(currentX + eachColumn - (c->leftMargin() + c->rightMargin() + c->width()));
				break;
			}

			switch (c->verticalAlignment())
			{
			case VerticalAlignment::Fill:
				c->setHeight(eachRow - (c->topMargin() + c->bottomMargin()));
				c->setPositionY(currentY);
				break;
			case VerticalAlignment::Top:
				c->setPositionY(currentY);
				break;
			case VerticalAlignment::Center:
				c->setPositionY(currentY + (eachRow - (c->topMargin() + c->bottomMargin() + c->height())) / 2);
				break;
			case VerticalAlignment::Bottom:
				c->setPositionY(currentY + eachRow - (c->topMargin() + c->bottomMargin() + c->height()));
				break;
			}


			ccount++;
			if (ccount>= cColumn)
			{
				ccount%= cColumn;
				currentY += eachRow;
				currentX = container.leftPadding();
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
		operator std::shared_ptr<Layout>()
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
	size_t m_columnCount = 2;
};



}