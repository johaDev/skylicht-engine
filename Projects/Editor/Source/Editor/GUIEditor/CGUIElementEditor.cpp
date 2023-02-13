/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CGUIElementEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"

namespace Skylicht
{
	namespace Editor
	{
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIElement);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIRect);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIRoundedRect);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIMask);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIImage);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUISprite);
		GUI_EDITOR_REGISTER(CGUIElementEditor, CGUIText);

		CGUIElementEditor::CGUIElementEditor()
		{

		}

		CGUIElementEditor::~CGUIElementEditor()
		{
		}

		void CGUIElementEditor::closeGUI()
		{
			CGUIEditor::closeGUI();
		}

		void CGUIElementEditor::initGUI(CGUIElement* gui, CSpaceProperty* ui)
		{
			CGUIEditor::initGUI(gui, ui);

			GUI::CCollapsibleGroup* group = ui->addGroup("GUI Basic", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_guiData, ui, layout);
			group->setExpand(true);
		}

		void CGUIElementEditor::update()
		{

		}

	}
}