#pragma once
#include "Miqui.Audio.BasicType.h"

namespace Miqui
{
namespace Audio
{



/* DrawerInfo for DrawMethod */
struct DrawerInfo
{
	Microsoft::WRL::ComPtr<IWICBitmap> bitmap;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> target;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

	D2D1_SIZE_F bitmapSize;
	int currentCount;
};

// States
enum class GraphDrawingModeType
{
	Linear,
	Stem,

};


//
//==================== Interface

enum class GraphSampleHandlerMode
{
	Pull,
	Peek,
	Push
};

/* Graph Sample Handler*/
template <typename _Ty>
struct IGraphSampleHandler
{
	virtual bool HandleSamples(_Ty * first, _Ty * last, GraphSampleHandlerMode mode) = 0;
};



//===========================================Graph Drawer's
/* GraphDrawer */
class AudioGraphControl;
struct BaseGraphDrawer
{
	virtual void Update(Miqui::StepTimer&) = 0;
	virtual void Draw(Miqui::DeviceResources&) = 0;

	virtual void CreateDeviceIndependentResources(Miqui::DeviceResources&) {}
	virtual void CreateDeviceDependentResources(Miqui::DeviceResources&) {}
	virtual void OnPointerEvent(Miqui::PointerEvent&) {}

	void GraphSampleHandler(IGraphSampleHandler<float> * handler) noexcept { m_graphSampleHandler = handler; }
	IGraphSampleHandler<float> * GraphSampleHandler() const noexcept { return m_graphSampleHandler; }
	void GraphSampleHandler(float* f, float*l, GraphSampleHandlerMode mode) noexcept { if (m_graphSampleHandler) m_graphSampleHandler->HandleSamples(f, l, mode); }

	void ControlRect(D2D1_RECT_F rect) noexcept;
	D2D1_RECT_F ControlRect() const noexcept;

	void ResizeBuffer(size_t size);

	void Parent(AudioGraphControl * parent) noexcept { m_parent = parent; }
	AudioGraphControl* Parent() const noexcept { return m_parent; }

	void Invalidate() noexcept;


protected:
	AudioGraphControl * m_parent;
	IGraphSampleHandler<float> * m_graphSampleHandler{ nullptr };
	miqs::array<float> m_sampleBuffer;
	D2D1_RECT_F m_controlRect{};
};

inline void BaseGraphDrawer::ControlRect(D2D1_RECT_F rect) noexcept { this->m_controlRect = rect; }
inline D2D1_RECT_F BaseGraphDrawer::ControlRect() const noexcept { return this->m_controlRect; }


//-------------------------------------------------------------
/* BitmapBasedGraphDrawer */
enum class BitmapBasedGraphDrawMethod
{
	StaticMode,
	RealtimeMode
};

struct BitmapBasedGraphDrawer: BaseGraphDrawer
{
	virtual void CreateDeviceDependentResources(Miqui::DeviceResources&) override;
	void BitmapSize(D2D1_SIZE_F size) noexcept;
	void CreateBitmap(UINT width, UINT height, Miqui::DeviceResources & resource);
	void Reset(D2D1::ColorF const& color);

	BitmapBasedGraphDrawMethod DrawMethod() const noexcept { return m_drawMethod; }
	void DrawMethod(BitmapBasedGraphDrawMethod method) noexcept { m_drawMethod = method; }


	virtual void Draw(Miqui::DeviceResources&) override;


protected:
	BitmapBasedGraphDrawMethod m_drawMethod{ BitmapBasedGraphDrawMethod::StaticMode };
	DrawerInfo m_drawerInfo;
};


/* SpectrogramBitmapBasedDrawer */
struct SpectrogramBitmapBasedDrawer: BitmapBasedGraphDrawer
{
	SpectrogramBitmapBasedDrawer();

	// Inherited via BitmapBasedGraphDrawer
	virtual void Update(Miqui::StepTimer &) override;

	ColorGradient& ColorGradientObject() noexcept { return m_colorGradient; }

private:
	ColorGradient m_colorGradient;
};

/* WaveformBitmapBasedDrawer */
struct WaveformBitmapBasedDrawer: BitmapBasedGraphDrawer
{
	// Inherited via BitmapBasedGraphDrawer
	virtual void Update(Miqui::StepTimer &) override;
};


//-------------------------------------------------------------
struct EditableWavetable: BaseGraphDrawer
{
	EditableWavetable();

	// Inherited via BaseGraphDrawer
	virtual void CreateDeviceIndependentResources(Miqui::DeviceResources&) override;
	virtual void CreateDeviceDependentResources(Miqui::DeviceResources&) override;
	virtual void Update(Miqui::StepTimer &) override;
	virtual void Draw(Miqui::DeviceResources &) override;
	virtual void OnPointerEvent(Miqui::PointerEvent&) override;

	void TableSize(size_t size) noexcept;
	size_t TableSize() const noexcept;

	std::vector<D2D1_POINT_2F> PointPositions() noexcept;

protected:
	void UpdateYAt(size_t index, float value);

private:
	typedef BaseGraphDrawer base_type;


	bool m_pressed{ false };


	float m_amplitudeRatio{ 0.9f };
	float m_sizeOfControlCircle{ 15.f };
	int m_selectedPoint{ -1 };
};

inline void EditableWavetable::TableSize(size_t size) noexcept
{
	this->m_sampleBuffer.reset(size, 0.f);
}

inline size_t EditableWavetable::TableSize() const noexcept
{
	return m_sampleBuffer.size();
}









//
//---------------------------------------------
/* AudioGraphControl */
class AudioGraphControl: public Miqui::IControl
{
	typedef Miqui::IControl base_type;
public:
	virtual void CreateDeviceIndependentResources(Miqui::DeviceResources&) override;
	virtual void CreateDeviceDependentResources(Miqui::DeviceResources&) override;
	//virtual void CreateWindowSizeDependentResources(Miqui::DeviceResources&) override {}
	//virtual void ReleaseDeviceDependentResources(Miqui::DeviceResources&) override {}
	//virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const &) override {}

	virtual void Update(Miqui::StepTimer&) override;
	virtual void Render(Miqui::DeviceResources&) override;

	void GraphDrawer(std::shared_ptr<BaseGraphDrawer> const& drawer);
	std::shared_ptr<BaseGraphDrawer> GraphDrawer() const noexcept;

	virtual void OnPointerEvent(Miqui::PointerEvent&) override;

	GraphDrawingModeType GraphDrawingMode() const noexcept { return m_graphDrawingMode; }
	void GraphDrawingMode(GraphDrawingModeType mode) noexcept { m_graphDrawingMode = mode; }

	std::pair<float, float> XLimits() const noexcept { return m_xlimits; }
	std::pair<float, float> YLimits() const noexcept { return m_ylimits; }

	void XLimits(std::pair<float, float> const& v) noexcept;
	void YLimits(std::pair<float, float> const& v) noexcept;
	void Limits(std::pair<float, float> const& x, std::pair<float, float> const& y) noexcept;

	bool UseXLimits() const noexcept { return m_useXLimits; }
	bool UseYLimits() const noexcept { return m_useYLimits; }
	
	void HideXLimits() noexcept;
	void HideYLimits() noexcept;

protected:
	virtual void OnSizeChanged() override;

private:
	std::shared_ptr<BaseGraphDrawer> m_graphDrawer;
	GraphDrawingModeType m_graphDrawingMode{ GraphDrawingModeType::Linear };


	std::pair<float, float> m_xlimits{}, m_ylimits{};
	bool m_useXLimits{ false }, m_useYLimits{ false };
};


}
}