//Copyright(c) 2009 - 2011, yakiimo02
//	All rights reserved.
//
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met :
//
//*Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
//	* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and / or other materials provided with the distribution.
//
//	* Neither the name of Yakiimo3D nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Original per-pixel linked list implementation code from Yakiimo02 was altered by Joao Raza and Gustavo Nunes for GPU Pro 5 'Screen Space Deformable Meshes via CSG with Per-pixel Linked Lists'
#include "pch.h"
#include "Keyboard.h"
#include "Camera.h"

using namespace DirectX;

Camera::Camera(float height, float width)
{
	m_leftRightRotation = 0.0f;
	m_upDownRotation = 0.0f;
	m_lastMousePos = XMFLOAT2(0.0f, 0.0f);
	m_Position = XMFLOAT3(0.0f, 0.0f, -10.0f);
		
	m_moveSpeed = 0.3f;

	XMStoreFloat4x4(&m_Proj, XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (float) height, 0.5f, 1000.0f) );
	
	UpdateViewMatrix();
}

void Camera::SetValues(const FXMVECTOR& pos, const FXMVECTOR forward, const FXMVECTOR& up, const float udrot, const float lrrot)
{
	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Forward, forward);
	XMStoreFloat3(&m_UpVector, up);
	
	m_upDownRotation    = udrot;
	m_leftRightRotation = lrrot;

	UpdateViewMatrix();
}

XMVECTOR Camera::GetPosition() const
{
	return XMLoadFloat3(&m_Position);
}
void _fastcall Camera::SetPosition(FXMVECTOR pos)
{
	XMStoreFloat3(&m_Position, pos);
	UpdateViewMatrix();
}

XMVECTOR Camera::GetForward() const
{
	return XMLoadFloat3(&m_Forward);
}

XMVECTOR Camera::GetUpVector() const
{
	return XMLoadFloat3(&m_UpVector);
}

XMMATRIX Camera::GetViewMatrix() const
{
	return XMLoadFloat4x4(&m_View);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMLoadFloat4x4(&m_Proj);
}

void Camera::UpdateViewMatrix()
{
	XMMATRIX rot = XMMatrixRotationX(m_upDownRotation) * XMMatrixRotationY(m_leftRightRotation);
	XMVECTOR originalTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR originalUP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotatedTarget = XMVector3Transform(originalTarget, rot);

	XMVECTOR postion = XMLoadFloat3(&m_Position);
	XMVECTOR forward = XMLoadFloat3(&m_Forward);
	XMVECTOR upVector = XMLoadFloat3(&m_UpVector);

	forward = postion + rotatedTarget;
	upVector = XMVector3Transform(originalUP, rot);
	
	XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(postion, forward, upVector));
	XMStoreFloat3(&m_Forward, forward);
	XMStoreFloat3(&m_UpVector, upVector);
}

void Camera::MoveCamera(const XMVECTOR& directionToMove)
{
	XMMATRIX rot = XMMatrixRotationX(m_upDownRotation) * XMMatrixRotationY(m_leftRightRotation);
	XMVECTOR rotatedVector = XMVector3Transform(directionToMove, rot);

	XMVECTOR position = XMLoadFloat3(&m_Position) + m_moveSpeed * rotatedVector;
	XMStoreFloat3(&m_Position, position);

	this->UpdateViewMatrix();
}
void Camera::SetMoveSpeed(float speed)
{
	m_moveSpeed = speed;
}

float Camera::GetMoveSpeed()
{
	return m_moveSpeed;
}


void Camera::Update(float delta)
{
	(void) delta; //! removes warning for delta not being used. included as parameter for future reference. 
	/*
	Keyboard::State keyState = Keyboard::Get().GetState();

	if (keyState.IsKeyDown(Keyboard::W) ) //& 0x8000
	{
		this->MoveCamera(XMVectorSet(0, 0, 1, 0));
	}
	if (keyState.IsKeyDown(Keyboard::A) )
	{
		this->MoveCamera(XMVectorSet(-1, 0, 0, 0));
	}
	if (keyState.IsKeyDown(Keyboard::S))
	{
		this->MoveCamera(XMVectorSet(0, 0, -1, 0));
	}
	if (keyState.IsKeyDown(Keyboard::D))
	{
		this->MoveCamera(XMVectorSet(1, 0, 0, 0));
	}
	*/
}

void Camera::RotateCamera(WPARAM buttonState, int x, int y)
{
	float rotationSpeed = 0.01f;
	
	if ((buttonState & MK_LBUTTON) != 0)
	{
		float previousX = (m_lastMousePos.x);
		float previousY = (m_lastMousePos.y);
		float deltaX = x - previousX;
		float deltaY = y - previousY;

		m_leftRightRotation = m_leftRightRotation + (rotationSpeed * deltaX);
		m_upDownRotation = m_upDownRotation + (rotationSpeed * deltaY);
		this->UpdateViewMatrix();
	}

	m_lastMousePos = XMFLOAT2(float(x), float(y));
}



