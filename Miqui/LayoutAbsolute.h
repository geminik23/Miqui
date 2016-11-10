#pragma once

#include "BasicType.h"

namespace Miqui
{


class AbsoluteLayoutProperties: public LayoutProperties
{
public:
	AbsoluteLayoutProperties() { this->resetPositionX(); this->resetPositionY(); }
	virtual ~AbsoluteLayoutProperties() {}

	float positionX() const { return m_positionX; }
	float positionY() const { return m_positionY; }
	void resetPositionX() { m_positionX = 0; }
	void resetPositionY() { m_positionY = 0; }
	void setPositionX(float posX) { this->m_positionX = posX; }
	void setPositionY(float posY) { this->m_positionY = posY; }


	// Builder
	struct Builder
	{
		Builder& posX(float x)
		{
			obj->setPositionX(x);
			return *this;
		}

		Builder& posY(float y)
		{
			obj->setPositionY(y);
			return *this;
		}

		std::shared_ptr<AbsoluteLayoutProperties> obj;

		operator std::shared_ptr<LayoutProperties>()
		{
			return obj;
		}
	};

	static Builder Create()
	{
		Builder b{ std::make_shared<AbsoluteLayoutProperties>() };
		return b;
	}

private:
	float m_positionX{};
	float m_positionY{};
};

class AbsoluteLayout:public Layout
{
public:
	virtual ~AbsoluteLayout() {}

	virtual void layout(ControlContainer&, std::list<IControl *>&) override {}

	virtual bool needUpdate() const override { return false; }

	virtual D2D1_POINT_2F queryPosition(IControl& control) override
	{
		LayoutProperties * layoutp = control.layoutProperties().get();
		AbsoluteLayoutProperties * ap = dynamic_cast<AbsoluteLayoutProperties *>(layoutp);
		if (ap == nullptr)
		{
			return{ control.translationX(),
				control.translationY() };
		}

		return{ ap->positionX() + control.translationX(),
			ap->positionY() + control.translationY() };
	}

	// Builder
	struct Builder
	{
		operator std::shared_ptr<Layout>()
		{
			return obj;
		}

		std::shared_ptr<AbsoluteLayout> obj;
	};

	static Builder Create()
	{
		Builder b{ std::make_shared<AbsoluteLayout>() };
		return b;
	}
};



}