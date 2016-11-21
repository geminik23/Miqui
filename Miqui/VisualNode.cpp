#pragma once
#include "pch.h"
#include "BasicType.h"

using namespace Miqui;

void VisualNode::CoreWindow(CoreControlWindow* window) noexcept
{
	this->m_corewindow = window;
}

void Miqui::VisualNode::ImmediateInvalidate() noexcept
{ if (m_corewindow)m_corewindow->ImmediateInvalidate(); }

void Miqui::VisualNode::Invalidate() noexcept
{ if(m_corewindow)m_corewindow->Invalidate();}

