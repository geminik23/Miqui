#pragma once

#include "BaseHeader.h"

namespace Miqui
{



struct StepTimer {
	std::chrono::duration<int64_t, std::ratio<1, 10'000'000>> duration;
};



// FOR CONTROL
enum class HorizontalAlignment
{
	Left, Right, Center, Fill
};
enum class VerticalAlignment
{
	Top, Bottom, Center, Fill
};

// LAYOUT
enum class LayoutOrientation
{
	TopToBottom, LeftToRight, RightToLeft, BottomToTop
};




enum class PointPropagationMode
{
	Full, // Full Point event propagation
	None, // No touch event propagation. 
	PassThrough // Skip node but propagate in sub tree. 
};

enum class PointMode
{
	Presssed,
	Moved,
	Released,
	Canceled,
	Exited
};

enum class PointerDeviceType
{
	Touch = 0, Pen = 1, Mouse = 2
};


//==== Base Interface


using DeviceResources = DxBase::DeviceResources;

class IVisualNode
{
public:
	virtual void CreateDeviceIndependentResources(DeviceResources& ) {}
	virtual void CreateDeviceDependentResources(DeviceResources&) {}
	virtual void CreateWindowSizeDependentResources(DeviceResources& ) {}
	virtual void ReleaseDeviceDependentResources(DeviceResources& ) {}
	virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const &) {}

	virtual void Update(StepTimer&) {}
	virtual void Render(DeviceResources& ) {}

};




// VisualNode

class VisualNode:public IVisualNode
{
public:
	virtual ~VisualNode() {}


	VisualNode* parent() { return m_parent; }
	void setParent(VisualNode * p) { m_parent = p; }

	D2D1_POINT_2F position() const { return m_position; }
	float positionX() const { return m_position.x; }
	float positionY() const { return m_position.y; }

	D2D1_SIZE_F size() const { return m_size; }
	float width() const { return m_size.width; }
	float height() const { return m_size.height; }

	float realPositionX() const { return translationX() + positionX(); }
	float realPositionY() const { return translationY() + positionY(); }


	float pivotX() const { return m_pivot.x; }
	float pivotY() const { return m_pivot.y; }
	float rotationZ() const { return m_rotationZ; }
	float scaleX() const { return m_scale.x; }
	float scaleY() const { return m_scale.y; }
	float translationX() const { return m_translation.x; }
	float translationY() const { return m_translation.y; }
	bool isVisible() const { return m_visible; }
	//PointPropagationMode pointPropagationMode() const { return m_pointPropagationMode; }


	void setPositionX(float positionX) { m_position.x = positionX; }
	void setPositionY(float positionY) { m_position.y = positionY; }
	void setPosition(float posX, float posY) { m_position.x = posX; m_position.y = posY; }
	void setPosition(D2D1_POINT_2F position) { m_position = position; }

	void setWidth(float width) { m_size.width = width; }
	void setHeight(float height) { m_size.height = height; }
	void setSize(float width, float height) { m_size.width = width; m_size.height = height; }
	void setSize(D2D1_SIZE_F size) { m_size = size; }

	void setPivot(float pivotX, float pivotY) { m_pivot.x = pivotX; m_pivot.y = pivotY; }
	void setPivotX(float pivotX) { m_pivot.x = pivotX; }
	void setPivotY(float pivotY) { m_pivot.y = pivotY; }
	void setRotationZ(float rotationZ) { m_rotationZ = rotationZ; }
	void setScale(float scaleX, float scaleY) { m_scale.x = scaleX; m_scale.y = scaleY; }
	void setScale(float scaleXY) { m_scale.x = m_scale.y = scaleXY; }
	void setScaleX(float scaleX) { m_scale.x = scaleX; }
	void setScaleY(float scaleY) { m_scale.y = scaleY; }
	void setTranslation(float translationX, float translationY) { m_translation.x = translationX; m_translation.y = translationY; }
	void setTranslationX(float translationX) { m_translation.x = translationX; }
	void setTranslationY(float translationY) { m_translation.y = translationY; }
	void setVisible(bool visible) { m_visible = visible; }
	//void setTouchPropagationMode(PointPropagationMode mode) { m_pointPropagationMode = mode; }

	void resetPivot() { resetPivotX(); resetPivotY(); }
	void resetPivotX() { m_pivot.x = 0; }
	void resetPivotY() { m_pivot.y = 0; }
	void resetRotationZ() { m_rotationZ = 0; }
	void resetScale() { resetScaleX(); resetScaleY(); }
	void resetScaleX() { m_scale.x = 1.0; }
	void resetScaleY() { m_scale.y = 1.0; }
	void resetTranslation() { resetTranslationX(); resetTranslationY(); }
	void resetTranslationX() { m_translation.x = 0; }
	void resetTranslationY() { m_translation.y = 0; }
	void resetVisible() { m_visible = true; }
	//void resetTouchPropagationMode() { m_pointPropagationMode = PointPropagationMode::Full; }


	void updateTranslation() { m_position.x += m_translation.x; m_position.y += m_translation.y; resetTranslation(); }



	D2D1_POINT_2F globalPosition()
	{//TODO
		D2D1_POINT_2F pos{};
		VisualNode * p = this;
		while (p)
		{
			pos.x += p->realPositionX();
			pos.y += p->realPositionY();
			p = p->parent();
		}

		return pos;
	}

private:
	//parent
	VisualNode * m_parent = nullptr;

	// translate
	D2D1_VECTOR_2F m_translation{ 0.0,0.0 };
	float m_rotationZ{ 0.0 };
	D2D1_POINT_2F m_pivot{ 0.0,0.0 };
	D2D1_VECTOR_2F m_scale{ 1.0,1.0 };

	//PointPropagationMode m_pointPropagationMode = PointPropagationMode::Full;

	D2D1_POINT_2F m_position{ 0.0,0.0 };
	D2D1_SIZE_F m_size{ 0.0,0.0 };
	bool m_visible = true;
};







//=== Container


class Container: public VisualNode
{
public:
	virtual ~Container() {}
	virtual void Update(StepTimer& st) { for (auto&n : m_nodes) { n->Update(st); } }

	virtual void Render(DeviceResources& resource)
	{
		auto context = resource.GetD2DDeviceContext();
		for (auto&n : m_nodes)
		{
			D2D1::Matrix3x2F originalM;
			context->GetTransform(&originalM);

			D2D1::Matrix3x2F currnet_matrix =
				D2D1::Matrix3x2F::Rotation(n->rotationZ(), D2D1_POINT_2F{ n->pivotX(), n->pivotY() })
				* D2D1::Matrix3x2F::Scale(D2D1_SIZE_F{ n->scaleX(), n->scaleY() } /*, scale_pivot*/)
				* D2D1::Matrix3x2F::Translation(n->positionX() + n->translationX(), n->positionY() + n->translationY())
				* originalM;
			context->SetTransform(currnet_matrix);
			n->Render(resource);
			context->SetTransform(originalM);
		}
	}

	virtual void CreateDeviceIndependentResources(DeviceResources& resource) override { for (auto&n : m_nodes)n->CreateDeviceIndependentResources(resource); }
	virtual void CreateDeviceDependentResources(DeviceResources& resource) override { for (auto&n : m_nodes)n->CreateDeviceDependentResources(resource); }
	virtual void CreateWindowSizeDependentResources(DeviceResources& resource) override { for (auto&n : m_nodes)n->CreateWindowSizeDependentResources(resource); }
	virtual void ReleaseDeviceDependentResources(DeviceResources& resource) override { for (auto&n : m_nodes)n->ReleaseDeviceDependentResources(resource); }
	virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const & di) override { for (auto&n : m_nodes)n->OnColorProfileChanged(di); }


	//virtual void onPointerEvent(PointEvent& e) override



	void remove(VisualNode * node) { node->setParent(nullptr); m_nodes.remove(node); }
	void removeAll()
	{
		for (auto&&n : m_nodes)	n->setParent(nullptr);
		m_nodes.clear();
	}
	void add(VisualNode * node) { m_nodes.push_back(node); node->setParent(this); }

private:
	std::list<VisualNode *> m_nodes;
};





//=========Layout

class IControl;
class ControlContainer;

struct Layout
{

	Layout();
	virtual ~Layout();
	virtual void layout(ControlContainer& container, std::list<IControl *>&) = 0;


	virtual D2D1_POINT_2F queryPosition(IControl& control);
	virtual bool needUpdate() const { return m_needUpdate; }
	virtual bool isInRegion(const D2D1_POINT_2F& point, IControl& control);
	virtual D2D1::Matrix3x2F getTransformation(IControl& control);
	virtual void requestUpdate() { m_needUpdate = true; }

protected:
	D2D1_SIZE_F getPossibleSize(IControl * control);

	void _arrangeComplete() { m_needUpdate = false; }
	bool m_needUpdate = true;


public:
	static void LayoutUpdate();
private:
	static std::list<Layout *> layout_container;
};

struct LayoutProperties
{
	virtual ~LayoutProperties() {}
};


class DefaultLayout: public Layout
{
public:
	virtual void layout(ControlContainer&, std::list<IControl *>&) { m_needUpdate = false; }
};


//=========================Control

class IControl: public VisualNode
{
public:
	virtual ~IControl() {}

	std::shared_ptr<LayoutProperties> layoutProperties() const { return m_layoutProperties; }

	bool isEnabled() const { return m_enabled; }

	float bottomMargin() const { return m_margin.w; }
	float topMargin() const { return m_margin.y; }
	float leftMargin() const { return m_margin.x; }
	float rightMargin() const { return m_margin.z; }

	float bottomPadding() const { return m_padding.w; }
	float topPadding() const { return m_padding.y; }
	float leftPadding() const { return m_padding.x; }
	float rightPadding() const { return m_padding.z; }

	HorizontalAlignment horizontalAlignment() const { return m_horizontalAlignment; }
	VerticalAlignment verticalAlignment() const { return m_verticalAlignment; }

	float minWidth() const { return m_minSize.width; }
	float minHeight() const { return m_minSize.height; }

	float maxWidth() const { return m_maxSize.width; }
	float maxHeight() const { return m_maxSize.height; }

	float preferredWidth() const { return m_preferredSize.width; }
	float preferredHeight() const { return m_preferredSize.height; }


	// set
	void setLayoutProperties(std::shared_ptr<LayoutProperties> lp) { this->m_layoutProperties = lp; }

	void setEnabled(bool e) { m_enabled = e; }

	void setBottomMargin(float v) { m_margin.w = v; }
	void setTopMargin(float v) { m_margin.y = v; }
	void setLeftMargin(float v) { m_margin.x = v; }
	void setRightMargin(float v) { m_margin.z = v; }

	void setBottomPadding(float v) { m_padding.w = v; }
	void setTopPadding(float v) { m_padding.y = v; }
	void setLeftPadding(float v) { m_padding.x = v; }
	void setRightPadding(float v) { m_padding.z = v; }

	void setHorizontalAlignment(HorizontalAlignment t) { m_horizontalAlignment = t; }
	void setVerticalAlignment(VerticalAlignment t) { m_verticalAlignment = t; }

	void setMinWidth(float v) { m_minSize.width = v; }
	void setMinHeight(float v) { m_minSize.height = v; }

	void setMaxWidth(float v) { m_maxSize.width = v; }
	void setMaxHeight(float v) { m_maxSize.height = v; }

	void setPreferredWidth(float v) { m_preferredSize.width = v; }
	void setPreferredHeight(float v) { m_preferredSize.height = v; }



	// reset
	void resetEnabled() { m_enabled = true; }

	void resetBottomMargin() { m_margin.w = 0; }
	void resetTopMargin() { m_margin.y = 0; }
	void resetLeftMargin() { m_margin.x = 0; }
	void resetRightMargin() { m_margin.z = 0; }

	void resetBottomPadding() { m_padding.w = 0; }
	void resetTopPadding() { m_padding.y = 0; }
	void resetLeftPadding() { m_padding.x = 0; }
	void resetRightPadding() { m_padding.z = 0; }

	void resetHorizontalAlignment() { m_horizontalAlignment = HorizontalAlignment::Left; }
	void resetVerticalAlignment() { m_verticalAlignment = VerticalAlignment::Top; }

	void resetMinWidth() { m_minSize.width = 0; m_isSetMinWidth = false; }
	void resetMinHeight() { m_minSize.height = 0; m_isSetMinHeight = false; }

	void resetMaxWidth() { m_maxSize.width = 0;  m_isSetMaxWidth = false; }
	void resetMaxHeight() { m_maxSize.height = 0;  m_isSetMaxHeight = false; }

	void resetPreferredWidth() { m_preferredSize.width = 0; m_isSetPreferredWidth = false; }
	void resetPreferredHeight() { m_preferredSize.height = 0; m_isSetPreferredHeight = false; }

	// check
	bool isMinWidthSet() const { return m_isSetMinWidth; }
	bool isMaxWidthSet() const { return m_isSetMaxWidth; }
	bool isMinHeightSet() const { return m_isSetMinHeight; }
	bool isMaxHeightSet() const { return m_isSetMaxHeight; }
	bool isPreferredWidthSet() const { return m_isSetPreferredWidth; }
	bool isPreferredHeightSet() const { return m_isSetPreferredHeight; }


	virtual void measuringSize();

	virtual void requestLayout();


private:
	HorizontalAlignment m_horizontalAlignment = HorizontalAlignment::Left;
	VerticalAlignment m_verticalAlignment = VerticalAlignment::Top;

	D2D1_VECTOR_4F m_margin{};
	D2D1_VECTOR_4F m_padding{};

	D2D1_SIZE_F m_minSize{};
	D2D1_SIZE_F m_maxSize{};
	D2D1_SIZE_F m_preferredSize{};
	bool m_isSetMinWidth = false,
		m_isSetMinHeight = false,
		m_isSetMaxWidth = false,
		m_isSetMaxHeight = false,
		m_isSetPreferredWidth = false,
		m_isSetPreferredHeight = false;

	bool m_enabled = true;

	std::shared_ptr<LayoutProperties> m_layoutProperties;
};







class ControlContainer: public IControl
{
public:
	ControlContainer():m_layout{ std::make_shared<DefaultLayout>() } {}
	virtual ~ControlContainer() {}



	virtual void Update(StepTimer& st) override
	{
		if (m_layout->needUpdate())
			m_layout->layout(*this, this->m_controls);

		for (auto&c : m_controls) { c->Update(st); }
	}

	virtual void CreateDeviceIndependentResources(DeviceResources& resource) override { for (auto&n : m_controls)n->CreateDeviceIndependentResources(resource); }
	virtual void CreateDeviceDependentResources(DeviceResources& resource) override { for (auto&n : m_controls)n->CreateDeviceDependentResources(resource); }
	virtual void CreateWindowSizeDependentResources(DeviceResources& resource) override { this->m_layout->requestUpdate(); for (auto&n : m_controls)n->CreateWindowSizeDependentResources(resource); }
	virtual void ReleaseDeviceDependentResources(DeviceResources& resource) override { for (auto&n : m_controls)n->ReleaseDeviceDependentResources(resource); }
	virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const & di) override { for (auto&n : m_controls)n->OnColorProfileChanged(di); }


	virtual void Render(DeviceResources& resource)  override
	{
		// get original matrix
		D2D1::Matrix3x2F originalM;
		auto context = resource.GetD2DDeviceContext();
		context->GetTransform(&originalM);
		for (auto it = m_controls.rbegin(); it != m_controls.rend(); ++it)
		{
			// get matrix from layout and transform
			D2D1::Matrix3x2F currnet_matrix = this->m_layout->getTransformation(*(*it)) * originalM;

			// update transform
			context->SetTransform(currnet_matrix);

			// render
			(*it)->Render(resource);
		}

		// recover transform
		context->SetTransform(originalM);
	}


	// layout
	void setLayout(std::shared_ptr<Layout> layout) { m_layout = layout; }
	std::shared_ptr<Layout> layout() { return m_layout; }



	void remove(IControl * c) { c->setParent(nullptr); m_controls.remove(c); }
	void removeAll()
	{
		for (auto&&n : m_controls)
		{
			n->setParent(nullptr);
		}
		m_controls.clear();
	}
	void add(IControl * c) { m_controls.push_back(c); c->setParent(this); }
	IControl* at(size_t i)
	{
		auto it = m_controls.begin();
		for (size_t ii{}; ii < i; ii++) it++;
		return *(it);
	}

	std::list<IControl *>& container() { return m_controls; }

private:
	std::list<IControl *> m_controls;
	std::shared_ptr<Layout> m_layout;
};








//=====CoreControlWindow

class CoreControlWindow: public DxBase::IRenderer
{
public:
	CoreControlWindow(const std::shared_ptr<DxBase::DeviceResources>& resource):IRenderer(resource) { InitializeResources(); }

	virtual void CreateDeviceIndependentResources() override { this->m_content.CreateDeviceIndependentResources(*m_deviceResources); }
	virtual void CreateDeviceDependentResources() override { this->m_content.CreateDeviceDependentResources(*m_deviceResources); }

	virtual void CreateWindowSizeDependentResources() override
	{
		auto size =m_deviceResources->GetD2DDeviceContext()->GetSize();
		m_content.setSize(size.width, size.height);
		this->m_content.CreateWindowSizeDependentResources(*m_deviceResources);
	}

	virtual void ReleaseDeviceDependentResources() override { this->m_content.ReleaseDeviceDependentResources(*m_deviceResources); }
	virtual void OnColorProfileChanged(_In_ winrt::Windows::Graphics::Display::DisplayInformation const&arg) override { this->m_content.OnColorProfileChanged(arg); }


	virtual void Draw() override
	{
		//Timer
		auto now = std::chrono::system_clock::now();
		Miqui::StepTimer timer;
		timer.duration = now - m_last;
		m_last = now;

		// Update First
		m_content.Update(timer);
		// Render
		m_content.Render(*m_deviceResources);

		m_deviceResources->Present();
	}

	Miqui::IControl* Content() noexcept
	{
		if (m_content.container().size() != 0) return m_content.container().front();
		return nullptr;
	}
	void Content(Miqui::IControl * control) noexcept
	{
		if (m_content.container().size() != 0) m_content.container().clear();
		m_content.container().push_back(control);

		if (m_deviceResources)
		{
			m_content.CreateDeviceIndependentResources(*m_deviceResources);
			m_content.CreateDeviceDependentResources(*m_deviceResources);
			m_content.CreateWindowSizeDependentResources(*m_deviceResources);
		}
	}

private:
	std::chrono::system_clock::time_point m_last;
	Miqui::ControlContainer m_content;
};



}