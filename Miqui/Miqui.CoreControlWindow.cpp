#include "pch.h"
#include "BasicType.h"
#include "LayoutGrid.h"
#include <sstream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>

using namespace Miqui;

using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Numerics;



PointerEvent::PointerEvent(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& arg, PointerMode mode)
{
	this->m_mode = mode;
	this->m_handled = false;
	this->m_virtualKeyModifies = static_cast<VirtualKeyModifiers>(arg.KeyModifiers());
	winrt::Windows::UI::Xaml::UIElement ele = sender.as<winrt::Windows::UI::Xaml::UIElement>();

	auto id = arg.GetCurrentPoint(ele).PointerId();
	auto points = arg.GetIntermediatePoints(ele);

	for (size_t i{}; i < points.Size(); ++i)
	{
		PointerPoint pp;
		auto point = points.GetAt(i);
		auto properties = point.Properties();


		pp.m_frameId = point.FrameId();
		pp.m_isInContact = point.IsInContact();
		pp.m_pointerId = point.PointerId();
		auto p = point.Position();
		pp.m_position = D2D1::Point2F(p.X, p.Y);

		pp.m_properties = PointerPointProperties{
			properties.IsBarrelButtonPressed(),
			properties.IsCanceled(),
			properties.IsEraser(),
			properties.IsHorizontalMouseWheel() ,
			properties.IsInverted() ,
			properties.IsLeftButtonPressed() ,
			properties.IsRightButtonPressed(),
			properties.IsPrimary() ,
			properties.IsXButton1Pressed() ,
			properties.IsXButton2Pressed()
		};
		pp.m_timestamp = point.Timestamp();

		this->m_pointerPoints.push_back(pp);
		// Check Current Point?
		if (pp.m_pointerId == id)
			this->m_current = m_pointerPoints.size() - 1;
	}
}




Miqui::CoreControlWindow::CoreControlWindow(const std::shared_ptr<DxBase::DeviceResources>& resource, DxBase::WinRTDx* base)
	:IRenderer(resource), m_base{ base }
{
	m_drawEvent.DeviceResource(resource);

	InitializeResources();

	m_content.Layout(std::make_shared<GridLayout>());
	m_content.CoreWindow(this);
	m_content.Parent(nullptr);
}


void Miqui::CoreControlWindow::OnPointerPressed(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e)
{
	auto p = PointerEvent(sender, e, PointerMode::Presssed);
	if (!CheckHandleNode(p)) m_content.OnPointerEvent(p);

	if (e.GetCurrentPoint(sender.as<winrt::Windows::UI::Xaml::UIElement>()).IsInContact() || p.Handled())e.Handled(true);
	if (p.Handled())
	{
		m_handledNode = p.m_handledNode;
	}
	handleRequestedInvalidation();
}

void Miqui::CoreControlWindow::OnPointerMoved(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &e)
{
	auto p = PointerEvent(sender, e, PointerMode::Moved);

	if (!CheckHandleNode(p)) m_content.OnPointerEvent(p);


	if (e.GetCurrentPoint(sender.as<winrt::Windows::UI::Xaml::UIElement>()).IsInContact() || p.Handled())e.Handled(true);

	if (p.Handled())
	{
		m_handledNode = p.m_handledNode;
	}
	handleRequestedInvalidation();
}

void Miqui::CoreControlWindow::OnPointerReleased(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &e)
{
	auto p = PointerEvent(sender, e, PointerMode::Released);
	if (!CheckHandleNode(p)) m_content.OnPointerEvent(p);

	if (e.GetCurrentPoint(sender.as<winrt::Windows::UI::Xaml::UIElement>()).IsInContact() || p.Handled())e.Handled(true);

	m_handledNode = nullptr;

	handleRequestedInvalidation();
}

void Miqui::CoreControlWindow::OnPointerCanceled(winrt::Windows::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const & e)
{
	auto p = PointerEvent(sender, e, PointerMode::Canceled);
	if (!CheckHandleNode(p)) m_content.OnPointerEvent(p);

	if (e.GetCurrentPoint(sender.as<winrt::Windows::UI::Xaml::UIElement>()).IsInContact() || p.Handled())e.Handled(true);

	m_handledNode = nullptr;

	handleRequestedInvalidation();
}

bool CoreControlWindow::CheckHandleNode(PointerEvent & pe)
{
	if (m_handledNode)
	{
		// change Position 
		auto gpos = m_handledNode->GlobalPosition();
	
		auto& current_pointer = (PointerPoint&)(pe.GetCurrentPoint());
		D2D1_POINT_2F init_pos = current_pointer.Position();

		current_pointer.m_position.x -= gpos.x;
		current_pointer.m_position.y -= gpos.y;

		m_handledNode->OnPointerEvent(pe);
		if (pe.Handled())
			pe.m_handledNode = m_handledNode;
		return true;
	}
	return false;
}

void Miqui::CoreControlWindow::handleRequestedInvalidation()
{
	if (m_invalidateRequested)
	{
		if (m_base) m_base->SafeDraw();
		m_invalidateRequested = true;
	}
}



















//=================================================


void D2CanvasEvent::DeviceResource(std::shared_ptr<DxBase::DeviceResources> resource) noexcept
{
	this->m_resource = resource;
}

void D2CanvasEvent::IndependentResource()
{

}

void D2CanvasEvent::DependentResource()
{
	if (!m_resource) return;
	auto ctx = m_resource->GetD2DDeviceContext();

	ctx->CreateSolidColorBrush({}, m_solidColorBrush.ReleaseAndGetAddressOf());

}


