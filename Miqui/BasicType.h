#pragma once

#include "BaseHeader.h"


namespace Miqui
{







struct StepTimer
{
	std::chrono::duration<int64_t, std::ratio<1, 10'000'000>> duration;
};



// FOR CONTROL
enum class HorizontalAlignmentType
{
	Left, Right, Center, Fill
};
enum class VerticalAlignmentType
{
	Top, Bottom, Center, Fill
};

// LAYOUT
enum class LayoutOrientation
{
	TopToBottom, LeftToRight, RightToLeft, BottomToTop
};


//============== Signal
template <typename ... _Args>
struct SignalHandler
{
	template <typename L> SignalHandler(L lambda):
		m_funct(lambda) {}

	template <typename F> SignalHandler(F * function) :
		RoutedEventHandler([=](auto && ... args) { function(args ...); })
	{}

	template <typename O, typename M> SignalHandler(O * object, M method) :
		RoutedEventHandler([=](auto && ... args) { ((*object).*(method))(args ...); })
	{}

	void operator()(_Args... args) { m_func(std::forward<_Args>(args)...); }

private:
	std::function<void(_Args...)> m_func;
};


struct SignalEventKey
{
	int id{};
};
template <typename ... _Args>
struct SignalEvent
{
	typedef SignalHandler<_Args...> Handler;
	typedef std::unordered_map<int, Handler> HandlerContainer;

	SignalEventKey Register(Handler const& handler)
	{
		this->m_container.insert({ m_id, handler });
		return{ m_id++ };
	}

	void Remove(SignalEventKey key) { if (this->m_container.find(key.id) != m_container.end()) m_container->erase(key.id); }

	void operator()(_Args... args) { 
		for (auto&& v : m_container) v.second(std::forward<_Args>(args)...);
		
	}

private:
	int m_id{};
	HandlerContainer m_container;
};





//============== For InputEvent
enum class PointPropagationModeType
{
	Full, // Full Point event propagation
	None, // No touch event propagation. 
	PassThrough // Skip node but propagate in sub tree. 
};

enum class PointerMode
{
	Presssed,
	Moved,
	Released,
	Canceled,
	Exited
};

enum class VirtualKeyModifiers
{
	None = 0,
	Control = 1,
	Menu = 2,
	Shift = 4,
	Windows = 8

};

enum class PointerDeviceType
{
	Touch = 0, Pen = 1, Mouse = 2
};


struct PointerPointProperties
{
	bool IsBarrelButtonPressed;
	bool IsCanceled;
	bool IsEraser;
	bool IsHorizontalMouseWheel;
	bool IsInverted;
	bool IsLeftButtonPressed;
	bool IsRightButtonPressed;
	bool IsPrimary;
	bool IsXButton1Pressed;
	bool IsXButton2Pressed;
};

class PointerPoint
{
public:
	PointerPoint() {}


	std::uint32_t FrameId() const { return m_frameId; }
	bool IsInContact() const { return m_isInContact; }
	//PointerDevice
	std::uint32_t PointerId() const { return m_pointerId; }
	D2D1_POINT_2F Position() const { return m_position; }
	const PointerPointProperties& Properties() const { return m_properties; }
	//D2D1_POINT_2F rawPosition() const {}
	std::uint64_t Timestamp() const { return m_timestamp; }


private:
	friend struct PointerEvent;
	friend class ControlContainer;
	friend class CoreControlWindow;

	std::uint32_t m_frameId;
	bool m_isInContact;
	std::uint32_t m_pointerId;
	D2D1_POINT_2F m_position;
	PointerPointProperties m_properties;
	std::uint64_t m_timestamp;
};



class IControl;

struct PointerEvent
{
	void Handled(bool handled) noexcept { m_handled = handled; }
	bool Handled() const noexcept { return m_handled; }

	VirtualKeyModifiers KeyModifiers() const noexcept { return m_virtualKeyModifies; }

	PointerEvent(PointerEvent&& other):
		m_handled{ other.m_handled },
		m_virtualKeyModifies{ other.m_virtualKeyModifies },
		m_current{ other.m_current },
		m_pointerPoints{ std::move(other.m_pointerPoints) }
	{}

	const PointerPoint& GetCurrentPoint() const noexcept { return m_pointerPoints[m_current]; }
	const std::vector<PointerPoint>& GetIntermediatePoints() const noexcept { return m_pointerPoints; }
	PointerMode Mode() const noexcept { return m_mode; }

private:
	friend class CoreControlWindow;
	friend class ControlContainer;

	PointerEvent(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&, PointerMode);

	PointerEvent(PointerEvent const& other) = delete;

	PointerMode m_mode;
	bool m_handled{ false };
	VirtualKeyModifiers m_virtualKeyModifies;

	std::vector<PointerPoint> m_pointerPoints;
	int m_current{};

	IControl* m_handledNode{ nullptr };

};


//==== Base Interface



using DeviceResources = DxBase::DeviceResources;

class IVisualNode
{
public:
	virtual void CreateDeviceIndependentResources(DeviceResources&) =0;
	virtual void CreateDeviceDependentResources(DeviceResources&) = 0;
	virtual void CreateWindowSizeDependentResources(DeviceResources&) = 0;
	virtual void ReleaseDeviceDependentResources(DeviceResources&) = 0;
	virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const &) = 0;

	virtual void Update(StepTimer&) = 0;
	virtual void Render(DeviceResources&) = 0;


	virtual void OnPointerEvent(PointerEvent &) = 0;

};




// VisualNode

class CoreControlWindow;
class VisualNode:public IVisualNode
{
public:
	virtual ~VisualNode() {}


	virtual VisualNode* Parent() { return m_parent; }
	virtual void Parent(VisualNode * p) { m_parent = p; }

	virtual D2D1_POINT_2F Position() const { return m_position; }
	virtual float PositionX() const { return m_position.x; }
	virtual float PositionY() const { return m_position.y; }

	virtual D2D1_SIZE_F Size() const { return m_size; }
	virtual float Width() const { return m_size.width; }
	virtual float Height() const { return m_size.height; }
	//FIXME AbsolutePosition
	virtual float RealPositionX() const { return TranslationX() + PositionX(); }
	virtual float RealPositionY() const { return TranslationY() + PositionY(); }


	virtual float PivotX() const { return m_pivot.x; }
	virtual float PivotY() const { return m_pivot.y; }
	virtual float RotationZ() const { return m_rotationZ; }
	virtual float ScaleX() const { return m_scale.x; }
	virtual float ScaleY() const { return m_scale.y; }
	virtual float TranslationX() const { return m_translation.x; }
	virtual float TranslationY() const { return m_translation.y; }
	virtual bool IsVisible() const { return m_visible; }
	virtual PointPropagationModeType PointPropagationMode() const { return m_pointPropagationMode; }


	virtual void PositionX(float positionX) { m_position.x = positionX; }
	virtual void PositionY(float positionY) { m_position.y = positionY; }
	virtual void Position(float posX, float posY) { m_position.x = posX; m_position.y = posY; }
	virtual void Position(D2D1_POINT_2F position) { m_position = position; }

	virtual void Width(float width) { m_size.width = width; OnSizeChanged();}
	virtual void Height(float height) { m_size.height = height;  OnSizeChanged();}
	virtual void Size(float width, float height) { m_size.width = width; m_size.height = height; OnSizeChanged();}
	virtual void Size(D2D1_SIZE_F size) { m_size = size; OnSizeChanged();}

	virtual void Pivot(float pivotX, float pivotY) { m_pivot.x = pivotX; m_pivot.y = pivotY; }
	virtual void PivotX(float pivotX) { m_pivot.x = pivotX; }
	virtual void PivotY(float pivotY) { m_pivot.y = pivotY; }
	virtual void RotationZ(float rotationZ) { m_rotationZ = rotationZ; }
	virtual void Scale(float scaleX, float scaleY) { m_scale.x = scaleX; m_scale.y = scaleY; }
	virtual void Scale(float scaleXY) { m_scale.x = m_scale.y = scaleXY; }
	virtual void ScaleX(float scaleX) { m_scale.x = scaleX; }
	virtual void ScaleY(float scaleY) { m_scale.y = scaleY; }
	virtual void Translation(float translationX, float translationY) { m_translation.x = translationX; m_translation.y = translationY; }
	virtual void TranslationX(float translationX) { m_translation.x = translationX; }
	virtual void TranslationY(float translationY) { m_translation.y = translationY; }
	virtual void Visible(bool visible) { m_visible = visible; }
	virtual void TouchPropagationMode(PointPropagationModeType mode) { m_pointPropagationMode = mode; }

	virtual void ResetPivot() { ResetPivotX(); ResetPivotY(); }
	virtual void ResetPivotX() { m_pivot.x = 0; }
	virtual void ResetPivotY() { m_pivot.y = 0; }
	virtual void ResetRotationZ() { m_rotationZ = 0; }
	virtual void ResetScale() { ResetScaleX(); ResetScaleY(); }
	virtual void ResetScaleX() { m_scale.x = 1.0; }
	virtual void ResetScaleY() { m_scale.y = 1.0; }
	virtual void ResetTranslation() { ResetTranslationX(); ResetTranslationY(); }
	virtual void ResetTranslationX() { m_translation.x = 0; }
	virtual void ResetTranslationY() { m_translation.y = 0; }
	virtual void ResetVisible() { m_visible = true; }
	virtual void ResetTouchPropagationMode() { m_pointPropagationMode = PointPropagationModeType::Full; }


	virtual void UpdateTranslation() { m_position.x += m_translation.x; m_position.y += m_translation.y; ResetTranslation(); }

	virtual void CoreWindow(CoreControlWindow* window) noexcept;
	virtual void ImmediateInvalidate() noexcept;
	virtual void Invalidate() noexcept;

	virtual D2D1_POINT_2F GlobalPosition()
	{//TODO
		D2D1_POINT_2F pos{};
		VisualNode * p = this;
		while (p)
		{
			pos.x += p->RealPositionX();
			pos.y += p->RealPositionY();
			p = p->Parent();
		}

		return pos;
	}


	void BackgroundColor(D2D_COLOR_F const& color)noexcept { m_background = color;}
	D2D_COLOR_F BackgroundColor() const noexcept { return m_background; }
	void ForegroundColor(D2D_COLOR_F const& color)noexcept { m_foreground = color; }
	D2D_COLOR_F ForegroundColor() const noexcept{ return m_foreground;}




	
	virtual void Update(StepTimer&) override {}
	virtual void Render(DxBase::DeviceResources& resource) override;

	virtual void CreateDeviceIndependentResources(DeviceResources&) override {}
	virtual void CreateDeviceDependentResources(DeviceResources& resource)override
	{
		auto ctx = resource.GetD2DDeviceContext();
		ctx->CreateSolidColorBrush(ForegroundColor(), this->m_solidColorBrush.ReleaseAndGetAddressOf());
	}
	virtual void CreateWindowSizeDependentResources(DeviceResources&) override {}
	virtual void ReleaseDeviceDependentResources(DeviceResources&) override {}
	virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const &) override {}
	virtual void OnPointerEvent(PointerEvent &) override {}




	// dx resource get functions
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> SolidColorBrush() const noexcept { return m_solidColorBrush; }


protected:
	// on changed functions
	virtual void OnSizeChanged() {}




	//parent
	VisualNode * m_parent{ nullptr };
	CoreControlWindow * m_corewindow{ nullptr };

	// color
	D2D_COLOR_F m_background{ 0.f,0.f,0.f,1.f};
	D2D_COLOR_F m_foreground{ 1.f,1.f,1.f,1.f };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_solidColorBrush;


	// translate
	D2D1_VECTOR_2F m_translation{ 0.0,0.0 };
	float m_rotationZ{ 0.0 };
	D2D1_POINT_2F m_pivot{ 0.0,0.0 };
	D2D1_VECTOR_2F m_scale{ 1.0,1.0 };

	PointPropagationModeType m_pointPropagationMode = PointPropagationModeType::Full;

	D2D1_POINT_2F m_position{ 0.0,0.0 };
	D2D1_SIZE_F m_size{ 0.0,0.0 };
	

	bool m_visible = true;
};



inline void VisualNode::Render(DxBase::DeviceResources& resource)
{
	auto ctx = resource.GetD2DDeviceContext();
	auto localsize = this->Size();
	D2D1_RECT_F rect;
	rect.left = -1;
	rect.top = -1;
	rect.right = localsize.width + 1;
	rect.bottom = localsize.height + 1;


	auto brush = this->SolidColorBrush();
	brush->SetColor(BackgroundColor());

	ctx->BeginDraw();
	ctx->FillRectangle(rect, brush.Get());
	ctx->EndDraw();
}





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
				D2D1::Matrix3x2F::Rotation(n->RotationZ(), D2D1_POINT_2F{ n->PivotX(), n->PivotY() })
				* D2D1::Matrix3x2F::Scale(D2D1_SIZE_F{ n->ScaleX(), n->ScaleY() } /*, scale_pivot*/)
				* D2D1::Matrix3x2F::Translation(n->PositionX() + n->TranslationX(), n->PositionY() + n->TranslationY())
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

	

	void Remove(VisualNode * node) { node->Parent(nullptr); node->CoreWindow(nullptr); m_nodes.remove(node); }
	void RemoveAll()
	{
		for (auto&&n : m_nodes)	n->Parent(nullptr);
		m_nodes.clear();
	}
	void Add(VisualNode * node) { m_nodes.push_back(node); node->Parent(this); node->CoreWindow(this->m_corewindow);}

private:
	std::list<VisualNode *> m_nodes;
};





//=========Layout

class IControl;
class ControlContainer;

struct BaseLayout
{

	BaseLayout();
	virtual ~BaseLayout();
	virtual void Layout(ControlContainer& container, std::list<IControl *>&) = 0;


	virtual D2D1_POINT_2F QueryPosition(IControl& control);
	virtual bool NeedUpdate() const { return m_needUpdate; }
	virtual bool IsInRegion(const D2D1_POINT_2F& point, IControl& control);
	virtual D2D1::Matrix3x2F Transformation(IControl& control);
	virtual void RequestUpdate() { m_needUpdate = true; }

protected:
	D2D1_SIZE_F GetPossibleSize(IControl * control);

	void _arrangeComplete() { m_needUpdate = false; }
	bool m_needUpdate = true;


public:
	static void LayoutUpdate();
private:
	static std::list<BaseLayout *> layout_container;
};

struct BaseLayoutProperties
{
	virtual ~BaseLayoutProperties() {}
};


class DefaultLayout: public BaseLayout
{
public:
	virtual void Layout(ControlContainer&, std::list<IControl *>&) { m_needUpdate = false; }
};


//=========================Control

class IControl: public VisualNode
{
public:
	virtual ~IControl() {}

	virtual std::shared_ptr<BaseLayoutProperties> LayoutProperties() const { return m_layoutProperties; }

	virtual bool IsEnabled() const { return m_enabled; }

	virtual float BottomMargin() const { return m_margin.w; }
	virtual float TopMargin() const { return m_margin.y; }
	virtual float LeftMargin() const { return m_margin.x; }
	virtual float RightMargin() const { return m_margin.z; }

	virtual float BottomPadding() const { return m_padding.w; }
	virtual float TopPadding() const { return m_padding.y; }
	virtual float LeftPadding() const { return m_padding.x; }
	virtual float RightPadding() const { return m_padding.z; }

	virtual HorizontalAlignmentType horizontalAlignment() const { return m_horizontalAlignment; }
	virtual VerticalAlignmentType verticalAlignment() const { return m_verticalAlignment; }

	virtual float MinWidth() const { return m_minSize.width; }
	virtual float MinHeight() const { return m_minSize.height; }

	virtual float MaxWidth() const { return m_maxSize.width; }
	virtual float MaxHeight() const { return m_maxSize.height; }

	virtual float PreferredWidth() const { return m_preferredSize.width; }
	virtual float PreferredHeight() const { return m_preferredSize.height; }


	// set
	virtual void LayoutProperties(std::shared_ptr<BaseLayoutProperties> lp) { this->m_layoutProperties = lp; }

	virtual void Enabled(bool e) { m_enabled = e; }

	virtual void BottomMargin(float v) { m_margin.w = v; }
	virtual void TopMargin(float v) { m_margin.y = v; }
	virtual void LeftMargin(float v) { m_margin.x = v; }
	virtual void RightMargin(float v) { m_margin.z = v; }

	virtual void BottomPadding(float v) { m_padding.w = v; }
	virtual void TopPadding(float v) { m_padding.y = v; }
	virtual void LeftPadding(float v) { m_padding.x = v; }
	virtual void RightPadding(float v) { m_padding.z = v; }

	virtual void HorizontalAlignment(HorizontalAlignmentType t) { m_horizontalAlignment = t; }
	virtual void VerticalAlignment(VerticalAlignmentType t) { m_verticalAlignment = t; }

	virtual void MinWidth(float v) { m_minSize.width = v; }
	virtual void MinHeight(float v) { m_minSize.height = v; }

	virtual void MaxWidth(float v) { m_maxSize.width = v; }
	virtual void MaxHeight(float v) { m_maxSize.height = v; }

	virtual void PreferredWidth(float v) { m_preferredSize.width = v; }
	virtual void PreferredHeight(float v) { m_preferredSize.height = v; }



	// reset
	virtual void ResetEnabled() { m_enabled = true; }

	virtual void ResetBottomMargin() { m_margin.w = 0; }
	virtual void ResetTopMargin() { m_margin.y = 0; }
	virtual void ResetLeftMargin() { m_margin.x = 0; }
	virtual void ResetRightMargin() { m_margin.z = 0; }

	virtual void ResetBottomPadding() { m_padding.w = 0; }
	virtual void ResetTopPadding() { m_padding.y = 0; }
	virtual void ResetLeftPadding() { m_padding.x = 0; }
	virtual void ResetRightPadding() { m_padding.z = 0; }

	virtual void ResetHorizontalAlignment() { m_horizontalAlignment = HorizontalAlignmentType::Left; }
	virtual void ResetVerticalAlignment() { m_verticalAlignment = VerticalAlignmentType::Top; }

	virtual void ResetMinWidth() { m_minSize.width = 0; m_isSetMinWidth = false; }
	virtual void ResetMinHeight() { m_minSize.height = 0; m_isSetMinHeight = false; }

	virtual void ResetMaxWidth() { m_maxSize.width = 0;  m_isSetMaxWidth = false; }
	virtual void ResetMaxHeight() { m_maxSize.height = 0;  m_isSetMaxHeight = false; }

	virtual void ResetPreferredWidth() { m_preferredSize.width = 0; m_isSetPreferredWidth = false; }
	virtual void ResetPreferredHeight() { m_preferredSize.height = 0; m_isSetPreferredHeight = false; }

	// check
	virtual bool IsMinWidthSet() const { return m_isSetMinWidth; }
	virtual bool IsMaxWidthSet() const { return m_isSetMaxWidth; }
	virtual bool IsMinHeightSet() const { return m_isSetMinHeight; }
	virtual bool IsMaxHeightSet() const { return m_isSetMaxHeight; }
	virtual bool IsPreferredWidthSet() const { return m_isSetPreferredWidth; }
	virtual bool IsPreferredHeightSet() const { return m_isSetPreferredHeight; }


	virtual void MeasuringSize();

	virtual void RequestLayout();


private:
	HorizontalAlignmentType m_horizontalAlignment = HorizontalAlignmentType::Left;
	VerticalAlignmentType m_verticalAlignment = VerticalAlignmentType::Top;

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

	std::shared_ptr<BaseLayoutProperties> m_layoutProperties;
};







class ControlContainer: public IControl
{
	typedef IControl base_type;
public:

	ControlContainer():m_layout{ std::make_shared<DefaultLayout>() } {}
	virtual ~ControlContainer() {}



	virtual void Update(StepTimer& st) override
	{
		if (m_layout->NeedUpdate())
			m_layout->Layout(*this, this->m_controls);

		for (auto&c : m_controls) { c->Update(st); }
	}

	virtual void CreateDeviceIndependentResources(DeviceResources& resource) override { base_type::CreateDeviceIndependentResources(resource); for (auto&n : m_controls)n->CreateDeviceIndependentResources(resource); }
	virtual void CreateDeviceDependentResources(DeviceResources& resource) override { base_type::CreateDeviceDependentResources(resource); for (auto&n : m_controls)n->CreateDeviceDependentResources(resource); }
	virtual void CreateWindowSizeDependentResources(DeviceResources& resource) override { base_type::CreateWindowSizeDependentResources(resource); this->m_layout->RequestUpdate(); for (auto&n : m_controls)n->CreateWindowSizeDependentResources(resource); }
	virtual void ReleaseDeviceDependentResources(DeviceResources& resource) override { base_type::ReleaseDeviceDependentResources(resource); for (auto&n : m_controls)n->ReleaseDeviceDependentResources(resource); }
	virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const & di) override { base_type::OnColorProfileChanged(di); for (auto&n : m_controls)n->OnColorProfileChanged(di); }


	virtual void Render(DeviceResources& resource)  override;
	virtual void OnPointerEvent(PointerEvent&) override;

	// layout
	virtual void Layout(std::shared_ptr<BaseLayout> layout) { m_layout = layout; }
	virtual std::shared_ptr<BaseLayout> Layout() { return m_layout; }



	virtual void Remove(IControl * c) { c->Parent(nullptr); c->CoreWindow(nullptr); m_controls.remove(c); }
	virtual void RemoveAll()
	{
		for (auto&&n : m_controls)
		{
			n->Parent(nullptr);
		}
		m_controls.clear();
	}
	virtual void Add(IControl * c) { m_controls.push_back(c); c->Parent(this); c->CoreWindow(this->m_corewindow);}
	virtual IControl* At(size_t i)
	{
		auto it = m_controls.begin();
		for (size_t ii{}; ii < i; ii++) it++;
		return *(it);
	}

	std::list<IControl *>& Container() { return m_controls; }
	virtual void CoreWindow(CoreControlWindow* window) noexcept override { VisualNode::CoreWindow(window); for (auto&&n : m_controls) n->CoreWindow(window); }

private:
	std::list<IControl *> m_controls;
	std::shared_ptr<BaseLayout> m_layout;
};








//=====ContentControl

class ContentControl;
struct IContentControl
{
	virtual void Update(Miqui::StepTimer&) = 0;
	virtual void Draw(Miqui::DeviceResources&) = 0;

	virtual void CreateDeviceIndependentResources(Miqui::DeviceResources&) = 0;
	virtual void CreateDeviceDependentResources(Miqui::DeviceResources&) = 0;

	virtual void OnPointerEvent(Miqui::PointerEvent&) {}


	void Parent(ContentControl * parent) noexcept { m_parent = parent; }
	ContentControl * Parent() const noexcept { return m_parent; }

	void ControlRect(D2D1_RECT_F rect) noexcept;
	D2D1_RECT_F ControlRect() const noexcept;

	void Invalidate() noexcept;

	virtual bool RequirePointEvent() noexcept { return false; }

protected:
	ContentControl * m_parent{ nullptr };
	D2D1_RECT_F m_controlRect{};
};






class ContentControl: public Miqui::IControl
{
public:
	virtual ~ContentControl() {}
	virtual void CreateDeviceIndependentResources(Miqui::DeviceResources&) override;
	virtual void CreateDeviceDependentResources(Miqui::DeviceResources&) override;
	//virtual void CreateWindowSizeDependentResources(Miqui::DeviceResources&) override;
	//virtual void ReleaseDeviceDependentResources(Miqui::DeviceResources&) override;
	//virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const &) override;

	virtual void Update(Miqui::StepTimer&timer) override;
	virtual void Render(Miqui::DeviceResources&) override;

	virtual void OnPointerEvent(Miqui::PointerEvent&) override;

	IContentControl* Content() const noexcept { return m_contentControl; }
	void Content(IContentControl * icc) noexcept;

protected:
	virtual void OnSizeChanged() override;

private:
	IContentControl * m_contentControl{ nullptr };
};









//=====CoreControlWindow

class CoreControlWindow: public DxBase::IRenderer
{
public:
	CoreControlWindow(const std::shared_ptr<DxBase::DeviceResources>& resource, DxBase::WinRTDx* base);

	virtual void CreateDeviceIndependentResources() override { this->m_content.CreateDeviceIndependentResources(*m_deviceResources); }
	virtual void CreateDeviceDependentResources() override { this->m_content.CreateDeviceDependentResources(*m_deviceResources); }

	virtual void CreateWindowSizeDependentResources() override
	{
		auto size = m_deviceResources->GetD2DDeviceContext()->GetSize();
		m_content.Size(size.width, size.height);
		this->m_content.CreateWindowSizeDependentResources(*m_deviceResources);
	}

	virtual void ReleaseDeviceDependentResources() override { this->m_content.ReleaseDeviceDependentResources(*m_deviceResources); }
	virtual void OnColorProfileChanged(_In_ winrt::Windows::Graphics::Display::DisplayInformation const&arg) override { this->m_content.OnColorProfileChanged(arg); }

	virtual void OnPointerPressed(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) override;
	virtual void OnPointerMoved(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) override;
	virtual void OnPointerReleased(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) override;
	virtual void OnPointerCanceled(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) override;


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
		if (m_content.Container().size() != 0) return m_content.Container().front();
		return nullptr;
	}
	void Content(Miqui::IControl * control) noexcept
	{
		if (m_content.Container().size() != 0) m_content.Container().clear();
		m_content.Add(control);

		if (m_deviceResources)
		{
			m_content.CreateDeviceIndependentResources(*m_deviceResources);
			m_content.CreateDeviceDependentResources(*m_deviceResources);
			m_content.CreateWindowSizeDependentResources(*m_deviceResources);
		}
	}

	void ImmediateInvalidate() noexcept { if (m_base)m_base->SafeDraw(); }
	void Invalidate() noexcept { m_invalidateRequested = true;}

	void SetWinRTDxBase(DxBase::WinRTDx* base) noexcept{ m_base = base;}


private:

	bool CheckHandleNode(PointerEvent& pe);
	
	void handleRequestedInvalidation();

	IControl * m_handledNode{ nullptr};
	std::chrono::system_clock::time_point m_last;
	Miqui::ControlContainer m_content;
	DxBase::WinRTDx* m_base{ nullptr };
	bool m_invalidateRequested{false};
};






}