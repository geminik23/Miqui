#include "pch.h"
#include "BasicType.h"

using namespace Miqui;

void Miqui::IControl::MeasuringSize()
{
	D2D1_SIZE_F size{ this->Width(), this->Height() };
	if (IsPreferredWidthSet()) size.width = this->PreferredWidth();
	if (IsPreferredHeightSet()) size.height = this->PreferredHeight();

	if (IsMinWidthSet() && size.width < this->MinWidth()) { size.width = MinWidth(); }
	if (IsMaxWidthSet() && size.width > this->MaxWidth()) { size.width = MaxWidth(); }
	if (IsMinHeightSet() && size.height < this->MinHeight()) { size.height = MinHeight(); }
	if (IsMaxHeightSet() && size.height > this->MaxHeight()) { size.height = MaxHeight(); }
	this->Width(size.width);
	this->Height(size.height);
}

void IControl::RequestLayout()
{
	BaseLayout::LayoutUpdate();
}


void Miqui::ControlContainer::Render(DeviceResources & resource)

{
	IControl::Render(resource);
	// get original matrix
	D2D1::Matrix3x2F originalM;
	auto context = resource.GetD2DDeviceContext();
	context->GetTransform(&originalM);
	for (auto it = m_controls.rbegin(); it != m_controls.rend(); ++it)
	{
		// get matrix from layout and transform
		D2D1::Matrix3x2F currnet_matrix = this->m_layout->Transformation(*(*it)) * originalM;

		// update transform
		context->SetTransform(currnet_matrix);

		// render
		(*it)->Render(resource);
	}

	// recover transform
	context->SetTransform(originalM);
}
//-------------
void ControlContainer::OnPointerEvent(PointerEvent& e)
{
	// save initial point size

	auto& current_pointer = (PointerPoint&)(e.GetCurrentPoint());
	D2D1_POINT_2F init_pos = current_pointer.Position();
	
	// set and send pointer_event
	for (auto it = m_controls.rbegin(); it != m_controls.rend(); ++it)
	{
		// check inside rect

		auto con_pos = m_layout->QueryPosition(*(*it));
		//D2D1_RECT_F r{ con_pos.x, con_pos.y, 0, 0 };
		//r.right = r.left + (*it)->width() - 1;
		//r.bottom = r.top + (*it)->height() - 1;

		current_pointer.m_position = init_pos;
		
		if (m_layout->IsInRegion(current_pointer.m_position, *((*it))))
		{
			current_pointer.m_position.x -= con_pos.x;
			current_pointer.m_position.y -= con_pos.y;


			(*it)->OnPointerEvent(e);

			if (e.Handled())
			{
				if (e.m_handledNode == nullptr)
				{
					e.m_handledNode = (*it);
				}
				return;
			}
		}
	}
	current_pointer.m_position = init_pos;
}










void ContentControl::CreateDeviceIndependentResources(Miqui::DeviceResources& resource)
{
	IControl::CreateDeviceIndependentResources(resource);
	if (m_contentControl)m_contentControl->CreateDeviceIndependentResources(resource);
}

void ContentControl::CreateDeviceDependentResources(Miqui::DeviceResources& resource)
{
	IControl::CreateDeviceDependentResources(resource);
	if (m_contentControl)m_contentControl->CreateDeviceDependentResources(resource);
}


void ContentControl::Update(Miqui::StepTimer&timer) { IControl::Update(timer); if (m_contentControl)m_contentControl->Update(timer); }

void ContentControl::Render(Miqui::DeviceResources& resource)
{
	//IControl::Render(resource);
	if (m_contentControl)m_contentControl->Draw(resource);
}

void ContentControl::OnPointerEvent(Miqui::PointerEvent& pe)
{
	if (this->m_contentControl && this->m_contentControl->RequirePointEvent()) this->m_contentControl->OnPointerEvent(pe);
}

void ContentControl::Content(IContentControl * icc) noexcept
{
	this->m_contentControl = icc;
	if (m_contentControl) m_contentControl->Parent(this);
	OnSizeChanged();
}

void ContentControl::OnSizeChanged()
{
	if (this->m_contentControl)
	{
		D2D1_RECT_F rect{ 0.f,0.f, Width(), Height() };
		m_contentControl->ControlRect(rect);
	}
}


void IContentControl::ControlRect(D2D1_RECT_F rect) noexcept { this->m_controlRect = rect; }
D2D1_RECT_F IContentControl::ControlRect() const noexcept { return this->m_controlRect; }
void IContentControl::Invalidate() noexcept { if (this->m_parent) m_parent->Invalidate(); }

