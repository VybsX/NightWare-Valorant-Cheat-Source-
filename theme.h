#include "main.h"
#include "icons.h"
#include "fonthelper.h"




void sextheme()
{

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMinSize = ImVec2(696, 437);
	style.WindowRounding = 7.0f;
	style.WindowPadding = ImVec2(0, 0);
	style.ChildRounding = 5.f;

	style.Colors[ImGuiCol_WindowBg] = ImColor{ 15, 15, 15, 240 };
	style.Colors[ImGuiCol_ChildBg] = ImColor{ 0, 0, 0, 0 };
	style.Colors[ImGuiCol_TitleBg] = ImColor{ 0, 0, 0, 0 };
	style.Colors[ImGuiCol_PopupBg] = ImColor{ 20, 20, 20, 240 };
	style.Colors[ImGuiCol_CheckMark] = ImColor{ 213, 0, 255, 138 };
	style.Colors[ImGuiCol_Text] = ImColor{ 255, 255, 255, 255 };
	style.Colors[ImGuiCol_TextDisabled] = ImColor{ 255, 255, 255, 255 };
	style.Colors[ImGuiCol_Border] = ImColor{ 86, 0, 92, 136 };
	style.Colors[ImGuiCol_BorderShadow] = ImColor{0, 0, 0, 0};
	style.Colors[ImGuiCol_FrameBg] = ImColor{ 76, 0, 92, 138 };
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor{ 127, 0, 152, 138 };
	style.Colors[ImGuiCol_FrameBgActive] = ImColor{ 127, 0, 152, 138 };
	style.Colors[ImGuiCol_SliderGrab] = ImColor{ 185, 0, 222, 138 };
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor{ 213, 0, 255, 138 };
	style.Colors[ImGuiCol_Button] = ImColor{ 213, 0, 255, 138 };
	style.Colors[ImGuiCol_ButtonHovered] = ImColor{ 213, 0, 255, 138 };
}


void TssakliMenu()
{


	static int tab = 0;
	ImGui::BeginGroup();

	ImGui::SetCursorPos(ImVec2(20, 40));
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Varsayılan fontu kullan
	ImGui::SetWindowFontScale(1.5f); // Yazıyı 2 kat büyüt
	ImGui::Text("nightware.pw");
	ImGui::PopFont();
	ImGui::SetWindowFontScale(1.0f); // Yazı boyutunu eski haline getir
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
	ImGui::SetCursorPos(ImVec2(145, 30));
	if (ImGui::Button(std::string(std::string(XorString(ICON_FA_EYE)) + XorString(" Visuals")).c_str(), ImVec2(120, 40))) {
		tab = 0;
	}

	if (ImGui::IsItemHovered()) {
		ImVec2 start = ImGui::GetItemRectMin();
		ImVec2 end = ImVec2(start.x + ImGui::GetItemRectSize().x, start.y);
		ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(255, 255, 255, 255));
	}
	ImGui::SetCursorPos(ImVec2(270, 30));
	if (ImGui::Button(std::string(std::string(XorString(ICON_FA_CROSSHAIRS)) + XorString(" Aimbot")).c_str(), ImVec2(120, 40))) {
		tab = 1;
	}
	if (ImGui::IsItemHovered()) {
		ImVec2 start = ImGui::GetItemRectMin();
		ImVec2 end = ImVec2(start.x + ImGui::GetItemRectSize().x, start.y);
		ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(255, 255, 255, 255));
	}
	ImGui::SetCursorPos(ImVec2(395, 30));
	if (ImGui::Button(std::string(std::string(XorString(ICON_FA_PENCIL)) + XorString(" Color")).c_str(), ImVec2(120, 40))) {
		tab = 2;
	}
	if (ImGui::IsItemHovered()) {
		ImVec2 start = ImGui::GetItemRectMin();
		ImVec2 end = ImVec2(start.x + ImGui::GetItemRectSize().x, start.y);
		ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(255, 255, 255, 255));
	}
	ImGui::SetCursorPos(ImVec2(520, 30));
	if (ImGui::Button(std::string(std::string(XorString(ICON_FA_FILE)) + XorString(" Config")).c_str(), ImVec2(120, 40))) {
		tab = 3;
	}
	if (ImGui::IsItemHovered()) {
		ImVec2 start = ImGui::GetItemRectMin();
		ImVec2 end = ImVec2(start.x + ImGui::GetItemRectSize().x, start.y);
		ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(255, 255, 255, 255));
	}
	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::SameLine(100);
	ImGui::EndGroup();
	switch (tab)
	{
	case 0 :
		ImGui::SetCursorPos(ImVec2(45, 80));
		ImGui::BeginGroup();
		{
		ImGui::Checkbox("Esp Box",&config.player_box);
		ImGui::Combo("Type", &config.esptype, type, IM_ARRAYSIZE(type));
		ImGui::Checkbox("Head Box", &config.circlehead);
		ImGui::Checkbox("Enemy View Angle", &config.player_view_angle);
		ImGui::Checkbox("Skeleton Esp", &config.player_skeleton);
		ImGui::Checkbox("Snapline", &config.player_snapline);
		ImGui::Checkbox("Distance", &config.player_distance);
		ImGui::Checkbox("HealthBar", &config.player_healthbar);
		//ImGui::Checkbox("Hit Sound", &config.hitsound);
		ImGui::Checkbox("Ignore Dormant", &config.player_ignore_dormant);
		ImGui::Checkbox("Glow", &config.glow);
		ImGui::Checkbox("Fov Changer", &config.fovchanger);

		ImGui::SliderFloat("Fov Value", &config.fovchangervalue, 100.f, 170.f, "%.f"); //i will make this to degrees later
		}
		ImGui::EndGroup();
		break;
	case 1:
		ImGui::SetCursorPos(ImVec2(45, 80));
		ImGui::BeginGroup();
		{
			ImGui::Checkbox("Aimbot", &config.aimenable);
			ImGui::Combo("Key", &config.aimbotkey, optKey, IM_ARRAYSIZE(optKey));
			ImGui::SliderFloat("Fov", &config.aimbot_fov, 10.f, 1000.f, "%.f"); //i will make this to degrees later
			ImGui::SliderFloat("Smooth", &config.aimbot_smooth, 1.f, 20.f, "%.f");
			ImGui::Combo("Target Bone", &config.aimboness, aimbone, IM_ARRAYSIZE(aimbone));
			ImGui::Checkbox("DrawFov", &config.aimbot_draw_fov);
			ImGui::Checkbox("RCS", &config.rcs);

		}
		ImGui::EndGroup();
		break;
	case 2:
		ImGui::SetCursorPos(ImVec2(45, 80));
		ImGui::BeginGroup();
		{

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0, 0, 0));
			ImGui::ColorEdit4("Esp Color", (float*)&config.espcolor);
			ImGui::ColorEdit4("Snapline Color", (float*)&config.snapcolor);
			ImGui::ColorEdit4("Skeleton Color", (float*)&config.skeleton);
			ImGui::ColorEdit4("Head Circle Color", (float*)&config.headboxcolor);
		}
		ImGui::EndGroup();
		break;
	case 3:
		ImGui::SetCursorPos(ImVec2(270, 120));
		ImGui::BeginGroup();
		{

			if (ImGui::Button("Save"))
			{
				SaveConfig();
				ImGui::Text("Config saved!");
			}
			if (ImGui::IsItemHovered()) {
				ImVec2 start = ImGui::GetItemRectMin();
				ImVec2 end = ImVec2(start.x + ImGui::GetItemRectSize().x, start.y);
				ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(255, 255, 255, 255));
			}
			ImGui::SetCursorPos(ImVec2(360, 120));
			if (ImGui::Button("Load"))
			{
				LoadConfig();
				ImGui::Text("Config loaded!");
			}
			if (ImGui::IsItemHovered()) {
				ImVec2 start = ImGui::GetItemRectMin();
				ImVec2 end = ImVec2(start.x + ImGui::GetItemRectSize().x, start.y);
				ImGui::GetWindowDrawList()->AddLine(start, end, ImColor(255, 255, 255, 255));
			}

			ImGui::SetCursorPos(ImVec2(185, 90));
			if (ImGui::BeginCombo("File", file_path.c_str())) {
				for (int i = 0; i < config_files.size(); i++) {
					bool is_selected = (file_path == config_files[i]);
					if (ImGui::Selectable(config_files[i], is_selected)) {
						file_path = "C:\\NightWare\\" + std::string(config_files[i]);
					}
				}
				ImGui::EndCombo();
			}



		}
		ImGui::EndGroup();
		break;
	}
}