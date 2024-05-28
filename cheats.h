
#include "theme.h"
#include "xor.h"

float Health;

uintptr_t root_comp;

uintptr_t pawn;


auto isVisible(DWORD_PTR mesh) -> bool
{
	float fLastSubmitTime = SEXread<float>(mesh + 0x378);
	float fLastRenderTimeOnScreen = SEXread<float>(mesh + 0x37C);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;

	return bVisible;
}

uintptr_t actor;
float Moruk = 0;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct State {
	uintptr_t keys[7];
};

namespace game
{
	int center_x = GetSystemMetrics(0) / 2 - 3;
	int center_y = GetSystemMetrics(1) / 2 - 3;

	float Width, Height = 0.f;

	//ue4 fvector class
	class fvector {
	public:
		fvector() : x(0.f), y(0.f), z(0.f) {}
		fvector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		~fvector() {}

		float x;
		float y;
		float z;

		inline float Dot(fvector v) {
			return x * v.x + y * v.y + z * v.z;
		}

		inline float distance(fvector v) {
			return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
		}

		fvector operator+(fvector v) {
			return fvector(x + v.x, y + v.y, z + v.z);
		}

		fvector operator-(fvector v) {
			return fvector(x - v.x, y - v.y, z - v.z);
		}
		fvector operator/(float flDiv) {
			return fvector(x / flDiv, y / flDiv, z / flDiv);
		}

		fvector operator*(float Scale) {

			return fvector(x * Scale, y * Scale, z * Scale);
		}
		inline float Length() {
			return sqrtf((x * x) + (y * y) + (z * z));
		}
		fvector operator-=(fvector v) {

			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
	};


	struct FVector2D
	{
	public:
		float x;
		float y;

		inline FVector2D() : x(0), y(0) {}
		inline FVector2D(float x, float y) : x(x), y(y) {}

		inline float Distance(FVector2D v) {
			return sqrtf(((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y)));
		}

		inline FVector2D operator+(const FVector2D& v) const {
			return FVector2D(x + v.x, y + v.y);
		}

		inline FVector2D operator-(const FVector2D& v) const {
			return FVector2D(x - v.x, y - v.y);
		}

	};


	//ue4 fquat class
	struct fquat {
		float x;
		float y;
		float z;
		float w;
	};

	//ue4 ftransform class
	struct ftransform {
		fquat rot;
		fvector translation;
		char pad[4];
		fvector scale;
		char pad1[4];
		D3DMATRIX ToMatrixWithScale() {
			D3DMATRIX m;
			m._41 = translation.x;
			m._42 = translation.y;
			m._43 = translation.z;

			float x2 = rot.x + rot.x;
			float y2 = rot.y + rot.y;
			float z2 = rot.z + rot.z;

			float xx2 = rot.x * x2;
			float yy2 = rot.y * y2;
			float zz2 = rot.z * z2;
			m._11 = (1.0f - (yy2 + zz2)) * scale.x;
			m._22 = (1.0f - (xx2 + zz2)) * scale.y;
			m._33 = (1.0f - (xx2 + yy2)) * scale.z;

			float yz2 = rot.y * z2;
			float wx2 = rot.w * x2;
			m._32 = (yz2 - wx2) * scale.z;
			m._23 = (yz2 + wx2) * scale.y;

			float xy2 = rot.x * y2;
			float wz2 = rot.w * z2;
			m._21 = (xy2 - wz2) * scale.y;
			m._12 = (xy2 + wz2) * scale.x;

			float xz2 = rot.x * z2;
			float wy2 = rot.w * y2;
			m._31 = (xz2 + wy2) * scale.z;
			m._13 = (xz2 - wy2) * scale.x;

			m._14 = 0.0f;
			m._24 = 0.0f;
			m._34 = 0.0f;
			m._44 = 1.0f;

			return m;
		}
	};
	fvector ControlRotation;

	//boring copy pasta math shit
	namespace math
	{
		D3DMATRIX matrix(fvector rot, fvector origin = fvector(0, 0, 0)) {
			float radPitch = (rot.x * float(M_PI) / 180.f);
			float radYaw = (rot.y * float(M_PI) / 180.f);
			float radRoll = (rot.z * float(M_PI) / 180.f);

			float SP = sinf(radPitch);
			float CP = cosf(radPitch);
			float SY = sinf(radYaw);
			float CY = cosf(radYaw);
			float SR = sinf(radRoll);
			float CR = cosf(radRoll);

			D3DMATRIX matrix;
			matrix.m[0][0] = CP * CY;
			matrix.m[0][1] = CP * SY;
			matrix.m[0][2] = SP;
			matrix.m[0][3] = 0.f;

			matrix.m[1][0] = SR * SP * CY - CR * SY;
			matrix.m[1][1] = SR * SP * SY + CR * CY;
			matrix.m[1][2] = -SR * CP;
			matrix.m[1][3] = 0.f;

			matrix.m[2][0] = -(CR * SP * CY + SR * SY);
			matrix.m[2][1] = CY * SR - CR * SP * SY;
			matrix.m[2][2] = CR * CP;
			matrix.m[2][3] = 0.f;

			matrix.m[3][0] = origin.x;
			matrix.m[3][1] = origin.y;
			matrix.m[3][2] = origin.z;
			matrix.m[3][3] = 1.f;

			return matrix;
		}

		D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
		{
			D3DMATRIX pOut;
			pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
			pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
			pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
			pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
			pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
			pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
			pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
			pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
			pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
			pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
			pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
			pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
			pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
			pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
			pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
			pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

			return pOut;
		}
	}


	//if one of the pointers is guarded, guard_(n) will be set to 'virtualaddy'. if its not guarded, it's 0 and + 0 doesnt do anything so idk just leave it
	namespace guarded_pointers
	{
		uintptr_t guard_local_pawn = 0x0;
		uintptr_t guard_controller = 0x0;
	}

	//maybe we need some pointers everywhere so we just store them here lmao (global)
	namespace pointer
	{
		uintptr_t local_pawn;
		uintptr_t local_pawn_old;
		uintptr_t player_controller;
		uintptr_t camera_manager;
		fvector camera_position;
	}

	//we need camera everywhere. lets store it here
	namespace camera
	{
		fvector location;
		fvector rotation;
		float fov;
	}

	//hell yeah we want different fonts for everything #COOL-ESP
	namespace fonts {
		ImFont* standard_font;
		ImFont* intro_font;
	}

	//we store the following info for each player so we are cooler
	struct player
	{
		uintptr_t for_actor;
		uintptr_t for_mesh;

		uintptr_t actor;
		uintptr_t mesh;
		uintptr_t bone_array;
		uintptr_t root_component;
		uintptr_t damage_handler;

		INT32 bone_count;
		INT32 ammo_count;

		std::string weapon_name;
		std::string agent_name;
		std::string player_name;

		float distance;
		float health;
		float shield;

		bool is_valid;
		bool is_damage_handler_guarded;
		bool is_mesh_guarded;
	};

	//create list of all players :D
	std::vector<player> player_pawns;

	//we need this operator, otherwise we can't pushback to our player list
	inline bool operator==(const player& a, const player& b) {
		if (a.actor == b.actor)
			return true;

		return false;
	}

	fvector CalcAngle(fvector src, fvector dst)
	{
		fvector angle;
		angle.x = -atan2f(dst.x - src.y, dst.y - src.y) / M_PI * 180.0f + 180.0f;
		angle.y = asinf((dst.z - src.z) / src.distance(dst)) * 180.0f / M_PI;
		angle.z = 0.0f;

		return angle;
	}

	fvector CalcAngleBackSideRotation(fvector src, fvector dst)
	{
		fvector angle;
		fvector delta = fvector((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

		double hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

		angle.x = atanf(delta.z / hyp) * (180.0f / hyp);
		angle.y = atanf(delta.y / delta.x) * (180.0f / M_PI);
		angle.z = 0;
		if (delta.x >= 0.0) angle.y += 180.0f;

		return angle;
	}

	void Clamp(fvector& Ang) {
		if (Ang.x < 0.f)
			Ang.x += 360.f;

		if (Ang.x > 360.f)
			Ang.x -= 360.f;

		if (Ang.y < 0.f) Ang.y += 360.f;
		if (Ang.y > 360.f) Ang.y -= 360.f;
		Ang.z = 0.f;
	}
	void draw_text(const char* content, ImVec2 screen_position, ImVec4 color, bool outlined, bool background) {
		if (background) {
			ImVec2 text_size = ImGui::CalcTextSize(content);
			ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(screen_position.x - 2, screen_position.y - 1), ImVec2(screen_position.x + text_size.x + 2, screen_position.y + text_size.y + 1), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 0.f, 0);
		}

		if (outlined) {
			ImGui::GetOverlayDrawList()->AddText(ImVec2(screen_position.x + 1, screen_position.y + 1), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), content);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(screen_position.x - 1, screen_position.y - 1), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), content);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(screen_position.x + 1, screen_position.y - 1), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), content);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(screen_position.x - 1, screen_position.y + 1), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), content);
		}

		ImGui::GetOverlayDrawList()->AddText(screen_position, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), content);
	}


	float SEXread_visible(player _player, int num)
	{
		return SEXread<float>(_player.for_mesh + _player.mesh + num);
	}


	//returns true if player is visible
	bool is_visible(player _player) {

		int i = 848;

		float v1 = SEXread_visible(_player, i);
		i += 0x8;
		float v2 = SEXread_visible(_player, i);

		float difference = v1 - v2;
		if (difference < 0.f)
		{
			difference *= -1;
		}

		if (difference < 0.01f)
			return true;

		return false;
	}

	//returns true if player is active
	bool is_dormant(player _player)
	{
		return SEXread<bool>(_player.for_actor + _player.actor + 0x100);
	}


	//returns agent name as a string
	std::string get_agent_name_by_id(int id) {
		switch (id)
		{
		case 14228437:
			return "NPC";
		case 14222427:
			return "Astra";
		case 14208983:
			return "Breach";
		case 14223790:
			return "Brimstone";
		case 14211583:
			return "Chamber";
		case 14216462:
			return "Cypher";
		case 14207406:
			return "Fade";
		case 14230110:
			return "Jett";
		case 14214159:
			return "Kay/O";
		case 14219028:
			return "Killjoy";
		case 14224708:
			return "Neon";
		case 14229275:
			return "Omen";
		case 14221452:
			return "Phoenix";
		case 14209558:
			return "Raze";
		case 14228484:
			return "Reyna";
		case 14227627:
			return "Sage";
		case 14217674:
			return "Sova";
		case 14220343:
			return "Viper";
		case 14226556:
			return "Yoru";
		default:
			return std::to_string(id);
		}
	}

	//returns player name as a string
	std::string get_player_name(int id) {
		return "";
	}
	//returns weapon name as a string
	std::string get_weapon_name_by_id(int id) {
		switch (id)
		{
		case 14228437:
			return "NPC";
		case 14222427:
			return "Astra";
		case 14208983:
			return "Breach";
		case 14223790:
			return "Brimstone";
		case 14211583:
			return "Chamber";
		case 14216462:
			return "Cypher";
		case 14207406:
			return "Fade";
		case 14230110:
			return "Jett";
		case 14214159:
			return "Kay/O";
		case 14219028:
			return "Killjoy";
		case 14224708:
			return "Neon";
		case 14229275:
			return "Omen";
		case 14221452:
			return "Phoenix";
		case 14209558:
			return "Raze";
		case 14228484:
			return "Reyna";
		case 14227627:
			return "Sage";
		case 14217674:
			return "Sova";
		case 14220343:
			return "Viper";
		case 14226556:
			return "Yoru";
		default:
			return std::to_string(id);
		}
	}

	//worldtoscreen what do i say xD
	fvector w2s(fvector world_location) {
		fvector ScreenLocation;
		D3DMATRIX tempMatrix = math::matrix(camera::rotation);
		fvector vAxisX, vAxisY, vAxisZ;
		vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
		fvector vDelta = world_location - camera::location;
		fvector vTransformed = fvector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
		if (vTransformed.z < .1f)
			vTransformed.z = .1f;
		float ScreenCenterX = center_x;
		float ScreenCenterY = center_y;
		if (!Moruk)
			camera::fov = 103;
		else
			camera::fov = Moruk;
		ScreenLocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(camera::fov * (float)M_PI / 360.f)) / vTransformed.z;
		ScreenLocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(camera::fov * (float)M_PI / 360.f)) / vTransformed.z;
		return ScreenLocation;
	}

	boolean in_rect(double centerX, double centerY, double radius, double x, double y) {
		return x >= centerX - radius && x <= centerX + radius &&
			y >= centerY - radius && y <= centerY + radius;
	}
	//function to create legit movements for aimbot
	void calculate_random_for_constant_aimbot()
	{
		while (true)
		{
			config.aim_constant_offset = 15.f;
			Sleep(200);

			config.aim_constant_offset = 2.f;
			Sleep(200);

			config.aim_constant_offset = 8.f;
			Sleep(200);

			config.aim_constant_offset = 20.f;
			Sleep(200);

			config.aim_constant_offset = 4.f;
			Sleep(200);

			config.aim_constant_offset = 12.f;
			Sleep(200);
		}

	}
	float CalculateDistance(int p1x, int p1y, int p2x, int p2y)
	{
		float diffY = p1y - p2y;
		float diffX = p1x - p2x;
		return sqrt((diffY * diffY) + (diffX * diffX));
	}

	//avg normalize function
	void normalize(fvector& in)
	{
		if (in.x > 89.f) in.x -= 360.f;
		else if (in.x < -89.f) in.x += 360.f;

		while (in.y > 180)in.y -= 360;
		while (in.y < -180)in.y += 360;
		in.z = 0;
	}

	void NormalizeAngles(fvector& angle)
	{
		while (angle.x > 89.0f)
			angle.x -= 180.f;

		while (angle.x < -89.0f)
			angle.x += 180.f;

		while (angle.y > 180.f)
			angle.y -= 360.f;

		while (angle.y < -180.f)
			angle.y += 360.f;
	}
	float GetFov(const fvector viewAngle, const fvector aimAngle)
	{
		fvector Delta = fvector(aimAngle.x - viewAngle.x, aimAngle.y - viewAngle.y, aimAngle.z - viewAngle.z);
		NormalizeAngles(Delta);
		return sqrt(pow(Delta.x, 2.0f) + pow(Delta.y, 2.0f));
	}

	//simple function to smooth angles
	fvector SmoothAim(fvector Camera_rotation, fvector Target, float SmoothFactor)
	{
		fvector diff = Target - Camera_rotation;
		normalize(diff);
		//
		return Camera_rotation + diff / SmoothFactor;
	}

	//converter
	float degree_to_radian(float degree)
	{
		return degree * (M_PI / 180);
	}


	//function to calculate an angle
	void angle_rotation(const fvector& angles, fvector* forward)
	{
		float	sp, sy, cp, cy;

		sy = sin(degree_to_radian(angles.y));
		cy = cos(degree_to_radian(angles.y));

		sp = sin(degree_to_radian(angles.x));
		cp = cos(degree_to_radian(angles.x));

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}


	fvector bone_matrix(int index, player _player)
	{
		size_t size = sizeof(ftransform);
		ftransform first_bone, comp_to_world;

		first_bone = SEXread<ftransform>(_player.bone_array + (size * index));
		comp_to_world = SEXread<ftransform>(_player.mesh + 0x250);


		D3DMATRIX matrix = math::MatrixMultiplication(first_bone.ToMatrixWithScale(), comp_to_world.ToMatrixWithScale());
		return fvector(matrix._41, matrix._42, matrix._43);
	}



	void RCS(fvector Target, fvector Camera_rotation, float SmoothFactor) {

		// Camera 2 Control space
		fvector ConvertRotation = Camera_rotation;
		normalize(ConvertRotation);

		// Calculate recoil/aimpunch
		auto ControlRotation = SEXread<fvector>(pointer::player_controller + 0x440);
		fvector DeltaRotation = ConvertRotation - ControlRotation;
		normalize(DeltaRotation);

		// Remove aimpunch from CameraRotation
		ConvertRotation = Target - (DeltaRotation * SmoothFactor);
		normalize(ConvertRotation);

		//Smooth the whole thing
		fvector Smoothed = SmoothAim(Camera_rotation, ConvertRotation, SmoothFactor);
		Smoothed -= (DeltaRotation / SmoothFactor);
		Clamp(Smoothed);
		normalize(Smoothed);
		SEXwrite<fvector>(pointer::player_controller + 0x440, (fvector)Smoothed);
		return;
	}
	//3d box for esp, etc.
	void draw_3d_box(fvector base, fvector top_reach, float wide, ImVec4 col, float thickness)
	{

		//calculate bottom rect
		fvector bottom_rect_1 = fvector(base.x + wide, base.y + wide, base.z);
		fvector bottom_rect_2 = fvector(base.x + wide, base.y - wide, base.z);
		fvector bottom_rect_3 = fvector(base.x - wide, base.y + wide, base.z);
		fvector bottom_rect_4 = fvector(base.x - wide, base.y - wide, base.z);

		//calculate top rect
		fvector top_rect_1 = fvector(top_reach.x + wide, top_reach.y + wide, top_reach.z);
		fvector top_rect_2 = fvector(top_reach.x + wide, top_reach.y - wide, top_reach.z);
		fvector top_rect_3 = fvector(top_reach.x - wide, top_reach.y + wide, top_reach.z);
		fvector top_rect_4 = fvector(top_reach.x - wide, top_reach.y - wide, top_reach.z);

		//w2s bottom rect
		bottom_rect_1 = w2s(bottom_rect_1);
		bottom_rect_2 = w2s(bottom_rect_2);
		bottom_rect_3 = w2s(bottom_rect_3);
		bottom_rect_4 = w2s(bottom_rect_4);

		//w2s top rect
		top_rect_1 = w2s(top_rect_1);
		top_rect_2 = w2s(top_rect_2);
		top_rect_3 = w2s(top_rect_3);
		top_rect_4 = w2s(top_rect_4);

		//render bottom rect
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_1.x, bottom_rect_1.y), ImVec2(bottom_rect_2.x, bottom_rect_2.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_2.x, bottom_rect_2.y), ImVec2(bottom_rect_4.x, bottom_rect_4.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_4.x, bottom_rect_4.y), ImVec2(bottom_rect_3.x, bottom_rect_3.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_3.x, bottom_rect_3.y), ImVec2(bottom_rect_1.x, bottom_rect_1.y), config.espcolor, thickness);

		//render top rect
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(top_rect_1.x, top_rect_1.y), ImVec2(top_rect_2.x, top_rect_2.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(top_rect_2.x, top_rect_2.y), ImVec2(top_rect_4.x, top_rect_4.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(top_rect_4.x, top_rect_4.y), ImVec2(top_rect_3.x, top_rect_3.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(top_rect_3.x, top_rect_3.y), ImVec2(top_rect_1.x, top_rect_1.y), config.espcolor, thickness);

		//render connection lines
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_1.x, bottom_rect_1.y), ImVec2(top_rect_1.x, top_rect_1.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_2.x, bottom_rect_2.y), ImVec2(top_rect_2.x, top_rect_2.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_3.x, bottom_rect_3.y), ImVec2(top_rect_3.x, top_rect_3.y), config.espcolor, thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom_rect_4.x, bottom_rect_4.y), ImVec2(top_rect_4.x, top_rect_4.y), config.espcolor, thickness);
	}

	void DrawFilledRect2(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0.0f);
	}


	void DrawNormalBox(int x, int y, int w, int h, int borderPx, ImColor color)
	{
		DrawFilledRect2(x + borderPx, y, w, borderPx, color);
		DrawFilledRect2(x + w - w + borderPx, y, w, borderPx, color);
		DrawFilledRect2(x, y, borderPx, h, color);
		DrawFilledRect2(x, y + h - h + borderPx * 2, borderPx, h, color);
		DrawFilledRect2(x + borderPx, y + h + borderPx, w, borderPx, color);
		DrawFilledRect2(x + w - w + borderPx, y + h + borderPx, w, borderPx, color);
		DrawFilledRect2(x + w + borderPx, y, borderPx, h, color);
		DrawFilledRect2(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);
	}

	fvector fhgfsdhkfshdghfsd205(fvector src, fvector dst)
	{
		fvector angle;
		angle.x = -atan2f(dst.x - src.x, dst.y - src.y) / M_PI * 180.0f + 180.0f;
		angle.y = asinf((dst.z - src.z) / src.distance(dst)) * 180.0f / M_PI;
		angle.z = 0.0f;

		return angle;
	}

	fvector CaadadalcAngle(fvector src, fvector dst)
	{
		fvector angle;
		fvector delta = fvector((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

		double hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

		angle.x = atanf(delta.z / hyp) * (180.0f / hyp);
		angle.y = atanf(delta.y / delta.x) * (180.0f / M_PI);
		angle.z = 0;
		if (delta.x >= 0.0) angle.y += 180.0f;

		return angle;
	}

	//todo
	void draw_corner_box(int x, int y, int w, int h, ImVec4 Color, int thickness) {
		float lineW = (w / 4.f);
		float lineH = (h / 4.f);
		ImDrawList* p = ImGui::GetOverlayDrawList();

		//oben links nach unten
		p->AddLine(ImVec2(x, y), ImVec2(x, y + lineH), config.espcolor, thickness);

		//oben links nach rechts (l-mittig)
		p->AddLine(ImVec2(x, y), ImVec2(x + lineW, y), config.espcolor, thickness);

		//oben rechts (r-mittig) nach rechts
		p->AddLine(ImVec2(x + w - lineW, y), ImVec2(x + w, y), config.espcolor, thickness);

		//oben rechts nach vert-rechts (oberhalb)
		p->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + lineH), config.espcolor, thickness);

		//unten vert-links (unterhalb) nach unten links
		p->AddLine(ImVec2(x, y + h - lineH), ImVec2(x, y + h), config.espcolor, thickness);

		//unten links nach rechts (mittig)
		p->AddLine(ImVec2(x, y + h), ImVec2(x + lineW, y + h), config.espcolor, thickness);

		//unten rechts (mittig) nach rechts
		p->AddLine(ImVec2(x + w - lineW, y + h), ImVec2(x + w, y + h), config.espcolor, thickness);

		//unten rechts nach vert-rechts (unterhalb)
		p->AddLine(ImVec2(x + w, y + h - lineH), ImVec2(x + w, y + h), config.espcolor, thickness);
	}

	//simple skeleton esp for each gender
	void draw_skeleton(player _player, ImVec4 col, float thickness)
	{
		fvector bone_head, bone_neck, bone_chest, bone_pelvis, bone_rshoulder, bone_relbow, bone_rhand, bone_rthigh, bone_rknee, bone_rfoot, bone_lshoulder, bone_lelbow, bone_lhand, bone_lthigh, bone_lknee, bone_lfoot;
		bone_head = bone_matrix(8, _player);
		bone_chest = bone_matrix(6, _player);
		bone_pelvis = bone_matrix(3, _player);


		if (_player.bone_count == 101) //female
		{
			bone_neck = bone_matrix(19, _player);

			bone_lshoulder = bone_matrix(21, _player);
			bone_lelbow = bone_matrix(40, _player);
			bone_lhand = bone_matrix(42, _player);

			bone_rshoulder = bone_matrix(46, _player);
			bone_relbow = bone_matrix(65, _player);
			bone_rhand = bone_matrix(67, _player);

			bone_lthigh = bone_matrix(78, _player);
			bone_lknee = bone_matrix(75, _player);
			bone_lfoot = bone_matrix(77, _player);

			bone_rthigh = bone_matrix(80, _player);
			bone_rknee = bone_matrix(82, _player);
			bone_rfoot = bone_matrix(84, _player);
		}
		else if (_player.bone_count == 103) //npc
		{
			bone_neck = bone_matrix(9, _player);

			bone_lshoulder = bone_matrix(33, _player);
			bone_lelbow = bone_matrix(30, _player);
			bone_lhand = bone_matrix(32, _player);

			bone_rshoulder = bone_matrix(58, _player);
			bone_relbow = bone_matrix(55, _player);
			bone_rhand = bone_matrix(57, _player);

			bone_lthigh = bone_matrix(63, _player);
			bone_lknee = bone_matrix(65, _player);
			bone_lfoot = bone_matrix(69, _player);

			bone_rthigh = bone_matrix(77, _player);
			bone_rknee = bone_matrix(79, _player);
			bone_rfoot = bone_matrix(83, _player);
		}
		else if (_player.bone_count == 104) //male
		{
			bone_neck = bone_matrix(19, _player);

			bone_lshoulder = bone_matrix(21, _player);
			bone_lelbow = bone_matrix(22, _player);
			bone_lhand = bone_matrix(23, _player);

			bone_rshoulder = bone_matrix(47, _player);
			bone_relbow = bone_matrix(48, _player);
			bone_rhand = bone_matrix(49, _player);

			bone_lthigh = bone_matrix(75, _player);
			bone_lknee = bone_matrix(76, _player);
			bone_lfoot = bone_matrix(78, _player);

			bone_rthigh = bone_matrix(82, _player);
			bone_rknee = bone_matrix(83, _player);
			bone_rfoot = bone_matrix(85, _player);
		}

		bone_head = w2s(bone_head);
		bone_neck = w2s(bone_neck);
		bone_chest = w2s(bone_chest);
		bone_pelvis = w2s(bone_pelvis);
		bone_lshoulder = w2s(bone_lshoulder);
		bone_lelbow = w2s(bone_lelbow);
		bone_lhand = w2s(bone_lhand);
		bone_rshoulder = w2s(bone_rshoulder);
		bone_relbow = w2s(bone_relbow);
		bone_rhand = w2s(bone_rhand);
		bone_lthigh = w2s(bone_lthigh);
		bone_lknee = w2s(bone_lknee);
		bone_lfoot = w2s(bone_lfoot);
		bone_rthigh = w2s(bone_rthigh);
		bone_rknee = w2s(bone_rknee);
		bone_rfoot = w2s(bone_rfoot);

		ImDrawList* draw = ImGui::GetOverlayDrawList();

		//top stuff
		draw->AddLine(ImVec2(bone_head.x, bone_head.y), ImVec2(bone_neck.x, bone_neck.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_neck.x, bone_neck.y), ImVec2(bone_chest.x, bone_chest.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_chest.x, bone_chest.y), ImVec2(bone_pelvis.x, bone_pelvis.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

		//right arm
		draw->AddLine(ImVec2(bone_chest.x, bone_chest.y), ImVec2(bone_rshoulder.x, bone_rshoulder.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_rshoulder.x, bone_rshoulder.y), ImVec2(bone_relbow.x, bone_relbow.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_relbow.x, bone_relbow.y), ImVec2(bone_rhand.x, bone_rhand.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

		//left arm
		draw->AddLine(ImVec2(bone_chest.x, bone_chest.y), ImVec2(bone_lshoulder.x, bone_lshoulder.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_lshoulder.x, bone_lshoulder.y), ImVec2(bone_lelbow.x, bone_lelbow.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_lelbow.x, bone_lelbow.y), ImVec2(bone_lhand.x, bone_lhand.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

		//right foot
		draw->AddLine(ImVec2(bone_pelvis.x, bone_pelvis.y), ImVec2(bone_rthigh.x, bone_rthigh.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_rthigh.x, bone_rthigh.y), ImVec2(bone_rknee.x, bone_rknee.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_rknee.x, bone_rknee.y), ImVec2(bone_rfoot.x, bone_rfoot.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

		//left foot
		draw->AddLine(ImVec2(bone_pelvis.x, bone_pelvis.y), ImVec2(bone_lthigh.x, bone_lthigh.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_lthigh.x, bone_lthigh.y), ImVec2(bone_lknee.x, bone_lknee.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
		draw->AddLine(ImVec2(bone_lknee.x, bone_lknee.y), ImVec2(bone_lfoot.x, bone_lfoot.y), ImGui::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	}

	///start from bottom (x, y), to top (w, h)
	void draw_health_bar(ImVec2 min, ImVec2 max, float health)
	{
		float health_percentage = health;
		health_percentage *= 0.01f;

		float lenght_left_to_right = max.x - min.x;
		lenght_left_to_right *= health_percentage;

		float healthbar_size_y = 5.f;

		float g = health_percentage * 255.f;
		float r = 255.f - g;
		float b = 0.f;

		r /= 255.f;
		g /= 255.f;
		b /= 255.f;

		//background
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(min.x, min.y - healthbar_size_y), ImVec2(max.x, max.y), ImGui::GetColorU32({ 0.1f, 0.1f, 0.1f, 1.f }), 0.f, 15);

		//health bar
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(min.x, min.y - healthbar_size_y), ImVec2(min.x + lenght_left_to_right, max.y), ImGui::GetColorU32({ r, g, b, 1.f }), 0.f, 15);

		//outline
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(min.x, min.y - healthbar_size_y), ImVec2(max.x, max.y), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 0.f, 15, 1.f);
	}



	//loop trough all actors via tarray
	void find() //find players
	{
		//ImGui::GetForegroundDrawList()->AddText(ImVec2(10, 10), ImColor(255, 35, 35), "nightware.pw");//sol yazı
		ImGuiIO& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Bahnschrift.ttf ", 15);

		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		ImFont* FontAwesome = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 14.f, &icons_config, icons_ranges);
		ImFont* FontAwesomeBig = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 19.f, nullptr, icons_ranges);
		ImFont* TitleFont = io.Fonts->AddFontFromMemoryCompressedTTF(trebucbd_compressed_data, trebucbd_compressed_size, 30, nullptr, io.Fonts->GetGlyphRangesDefault());



		while (true)
		{
			Sleep(5);

			world = SEXread<uintptr_t>(virtualaddy + 0x60);
			world = check::validate_pointer(world);
			if (!world) continue;

			game_instance = SEXread2<uintptr_t>(world + 0x1A0);//offset 0x1a0
			if (!game_instance) continue;

			persistent_level = SEXread2<uintptr_t>(world + 0x38);
			persistent_level = check::validate_pointer(persistent_level);
			if (!persistent_level) continue;

			uintptr_t local_players = SEXread2<uintptr_t>(game_instance + 0x40);//offset 0x40
			if (!local_players) continue;

			local_player = SEXread2<uintptr_t>(local_players);
			if (!local_player) continue;

			Sleep(500);

			pointer::player_controller = SEXread2<uintptr_t>(local_player + 0x38);

			ControlRotation = SEXread<fvector>(pointer::player_controller + offsets::control_rotation);

			pawn = SEXread2<DWORD_PTR>(pointer::player_controller + offsets::apawn);
			playerstate = SEXread2<uintptr_t>(pawn + 0x3F0);
			teamComponent = SEXread2<uintptr_t>(playerstate + 0x628);
			teamID = SEXread2<int>(teamComponent + offsets::team_id);


			uintptr_t local_pawn = SEXread2<uintptr_t>(pointer::player_controller + 0x460);

			pointer::local_pawn = local_pawn;

			pointer::camera_manager = SEXread2<uintptr_t>(pointer::player_controller + 0x478);//offsets player_camera 0x478
			pointer::camera_position = SEXread<fvector>(pointer::camera_manager + 0x1120);


			uintptr_t actor_array = SEXread2<uintptr_t>(persistent_level + 0xA0);//offset 0xa0
			if (!actor_array) continue;

			actor_count = SEXread2<INT32>(persistent_level + 0xA8);
			if (!actor_count) continue;


			for (int x = 0; x < actor_count; x++)
			{

				actor = SEXread2<uintptr_t>(actor_array + (x * 0x8));
				if (!actor) continue;

				INT32 unique_id = SEXread2<INT32>(actor + 0x38);
				if (unique_id != 18743553) continue;

				uintptr_t mesh = SEXread2<uintptr_t>(actor + 0x430);//offset 0x430
				if (!mesh) continue;

				IsVisible = isVisible(mesh);

				uintptr_t playerstate = SEXread2<uintptr_t>(actor + offsets::player_state);
				uintptr_t team_component = SEXread2<uintptr_t>(playerstate + offsets::team_component);
				int team_id = SEXread2<int>(team_component + offsets::team_id);
				int mybone_count = SEXread2<int>(mesh + offsets::bone_count);
				bool is_bot = mybone_count == 103;
				if (team_id == teamID && !is_bot) {
					continue;
				}

				root_comp = SEXread2<uintptr_t>(actor + 0x230);
				if (!root_comp) continue;

				uintptr_t damage_handler = SEXread2<uintptr_t>(actor + 0xa10);//offset 0xa10
				if (!damage_handler) continue;

				Health = SEXread2<float>(damage_handler + 0x1B0);

				if (Health <= 0) continue;

				uintptr_t bone_array = SEXread2<uintptr_t>(mesh + 0x5c0);
				if (!bone_array) continue;


				INT32 bone_count = SEXread2<INT32>(mesh + 0x5c8);
				if (!bone_count) continue;




				{
					uintptr_t actor;
					uintptr_t mesh;
					uintptr_t bone_array;
					uintptr_t root_component;
					uintptr_t damage_handler;

					INT32 bone_count;
					INT32 ammo_count;

					std::string weapon_name;
					std::string agent_name;
					std::string player_name;

					float distance;
					float health;
					float shield;

					bool is_valid;
					bool is_damage_handler_guarded;
					bool is_mesh_guarded;
				};
				uintptr_t for_actor = 0x1, for_mesh = 0x1;
				bool is_damage_handler_guarded = false, is_mesh_guarded = false;
				//define player
				player this_player{
					for_actor, //guarded region ptr
					for_mesh, //guarded region ptr
					actor,
					mesh,
					bone_array,
					root_comp,
					damage_handler,
					bone_count,
					0, //ammo count
					"", //weapon name
					"", //agent name
					"", //player name
					0.f, //distance
					Health, //health
					0.f, //shleid
					true,
					is_damage_handler_guarded,
					is_mesh_guarded
				};

				//pushback player (if he isnt in the actor list yet)
				if (!player_pawns.empty()) {
					auto found_player = std::find(player_pawns.begin(), player_pawns.end(), this_player);
					if (found_player == player_pawns.end())
					{
						player_pawns.push_back(this_player);
					}


				}
				else
				{
					player_pawns.push_back(this_player);
				}

			}
			Sleep(500);
		}
	}

#define DegreeToRadian( Degree )	( ( Degree ) * ( M_PI / 180.0f ) )


	inline FVector2D WorldRadar(fvector srcPos, fvector distPos, float yaw, float radarX, float radarY, float size)
	{
		auto cosYaw = cos(DegreeToRadian(-yaw));
		auto sinYaw = sin(DegreeToRadian(-yaw));

		auto deltaX = srcPos.x - distPos.x;
		auto deltaY = srcPos.y - distPos.y;

		auto locationX = (float)(deltaY * cosYaw - deltaX * sinYaw) / 45.f;
		auto locationY = (float)(deltaX * cosYaw + deltaY * sinYaw) / 45.f;

		if (locationX > (size - 2.f))
			locationX = (size - 2.f);
		else if (locationX < -(size - 2.f))
			locationX = -(size - 2.f);

		if (locationY > (size - 6.f))
			locationY = (size - 6.f);
		else if (locationY < -(size - 6.f))
			locationY = -(size - 6.f);

		return FVector2D((int)(-locationX + radarX), (int)(locationY + radarY));
	}

	static fvector pRadar;

	void DrawRadar(fvector EntityPos)
	{
		RGBA espcol = { 0, 143, 209,255 };

		auto radar_posX = pRadar.x + 135;
		auto radar_posY = pRadar.y + 135;



		if (pawn == 0) return;
		uint64_t rot = SEXread<uint64_t>(pointer::player_controller + offsets::camera_rotation);

		fvector LocalPos = SEXread<fvector>(root_comp + offsets::root_position);

		auto Radar2D = WorldRadar(LocalPos, EntityPos, rot, radar_posX, radar_posY, 135.f);
		DrawCircleFilled2(Radar2D.x, Radar2D.y, 4, &espcol);
	}
	auto TeamID(uintptr_t APawn) -> int {
		auto PlayerState = SEXread<uintptr_t>(APawn + offsets::player_state);
		auto TeamComponent = SEXread<uintptr_t>(PlayerState + offsets::team_component);
		return SEXread<int>(TeamComponent + offsets::team_id);
	};
	void update_camera()
	{
		camera::location = SEXread<fvector>(pointer::camera_manager + 0x1260);
		camera::rotation = SEXread<fvector>(pointer::camera_manager + 0x126C);

	}


	bool teamswap = false;


	void render_players()
	{
		Moruk = SEXread<float>(pointer::camera_manager + 0x3f4);

		if (config.fovchanger)
			SEXwrite<float>(pointer::camera_manager + 0x3f4, config.fovchangervalue);
		update_camera();


		auto isFrames = ImGui::GetFrameCount();

		static float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;

		ImVec4 isRGB = ImVec4(isRed, isGreen, isBlue, 1.0f);



		if (isFrames % 1 == 0) //We use modulus to check if it's divisible by 1, and if the remainder equals 0, then we continue. This effect gets called every frame.
		{

			if (isGreen == 0.01f && isBlue == 0.0f)
			{
				isRed += 0.01f;

			}

			if (isRed > 0.99f && isBlue == 0.0f)
			{
				isRed = 1.0f;

				isGreen += 0.01f;

			}

			if (isGreen > 0.99f && isBlue == 0.0f)
			{
				isGreen = 1.0f;

				isRed -= 0.01f;

			}

			if (isRed < 0.01f && isGreen == 1.0f)
			{
				isRed = 0.0f;

				isBlue += 0.01f;

			}

			if (isBlue > 0.99f && isRed == 0.0f)
			{
				isBlue = 1.0f;

				isGreen -= 0.01f;

			}

			if (isGreen < 0.01f && isBlue == 1.0f)
			{
				isGreen = 0.0f;

				isRed += 0.01f;

			}

			if (isRed > 0.99f && isGreen == 0.0f)
			{
				isRed = 1.0f;

				isBlue -= 0.01f;

			}

			if (isBlue < 0.01f && isGreen == 0.0f)
			{
				isBlue = 0.0f;

				isRed -= 0.01f;

				if (isRed < 0.01f)
					isGreen = 0.01f;

			}

		}



		int closestplayer = 1337;
		float closest_distance = FLT_MAX;

		for (int x = 0; x < player_pawns.size(); x++)
		{
			player this_player = player_pawns[x];


			float health = 0;
			health = SEXread<float>(this_player.damage_handler + 0x1B0);

			if (health <= 0.f || health > 999.f)
			{
				player_pawns[x].is_valid = false;
			}

			if (!this_player.is_valid)
			{
				auto erase_player = std::find(player_pawns.begin(), player_pawns.end(), this_player);
				player_pawns.erase(erase_player);
				continue;
			}

			fvector zero = bone_matrix(0, this_player);
			fvector head = bone_matrix(8, this_player);

			fvector zero_r = fvector(zero.x, zero.y, zero.z - 5);

			fvector vBaseBoneOut = w2s(fvector(zero.x, zero.y, zero.z));
			fvector vBaseBoneOut2 = w2s(fvector(zero.x, zero.y, zero.z - 15));

			fvector vHeadBoneOut = w2s(fvector(head.x, head.y, head.z));

			fvector vHeadBoneOut2 = w2s(fvector(head.x, head.y, head.z + 15));

			float BoxHeight = abs(vHeadBoneOut2.y - vBaseBoneOut.y);
			float BoxWidth = BoxHeight * 0.55;

			fvector head_r = fvector(head.x, head.y, head.z + 20);
			fvector head_r_2 = fvector(head.x, head.y, head.z + 30);

			fvector zero_w2s = w2s(zero);
			fvector head_w2s = w2s(head);

			fvector zero_w2s_r = w2s(zero_r);
			fvector head_w2s_r = w2s(head_r);

			fvector head_w2s_r_2 = w2s(head_r_2);

			int Width = GetSystemMetrics(SM_CXSCREEN);
			int Height = GetSystemMetrics(SM_CYSCREEN);


			bool active = is_dormant(this_player);
			if (config.player_ignore_dormant && !active)
				continue;


			if (config.circlehead == true)
			{
				ImGui::GetOverlayDrawList()->AddCircle(ImVec2(vHeadBoneOut.x, vHeadBoneOut.y), BoxWidth / 5, ImColor(config.headboxcolor), 64, 1.f);
			}

			if (config.glow)
				SEXwrite<float>(this_player.actor + 0x6f0, 100);


			if (config.player_healthbar)
			{
				float BoxHeight = (zero_w2s_r.y - head_w2s_r.y) / 10.f;
				float BoxWidth = BoxHeight / 2.f;
				float BoxHeight2 = abs(zero_w2s_r.y - head_w2s_r.y);
				float BoxWidth2 = BoxHeight * 0.50;

				float x1 = zero_w2s_r.x - BoxWidth - (BoxHeight2 * 0.35);
				float x2 = zero_w2s_r.x - (BoxHeight2 * 0.35);
				float y1 = head_w2s_r.y;
				float y2 = zero_w2s_r.y;

				draw_health_bar(ImVec2(x1, y1), ImVec2(x2, y2), health);
			}

			//if (config.hitsound)
			//{
			//	if (health - 1)
			//	{
			//		Beep(1000, 100);
			//	}
			//}


			bool visible = is_visible(this_player);



			char healthh[64];
			sprintf_s(healthh, "[H: %.f] ", health);
			std::string Out69 = healthh;

			Out69 = "[H: " + std::to_string(static_cast<int32_t>(health)) + "]";
			ImVec2 TextSize = ImGui::CalcTextSize(Out69.c_str());

			if (writehealth)
			{
				ImGui::GetOverlayDrawList()->AddText(ImVec2(vBaseBoneOut2.x - TextSize.x / 2, vBaseBoneOut2.y + 5 - TextSize.y / 2), ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.f }), Out69.c_str());
			}


			int bottom_text_offset = 2;

			if (genderesp)
			{
				if (this_player.bone_count = 101)
				{
					float distance = camera::location.distance(zero) / 100.f;
					char distance_text[256];
					sprintf_s(distance_text, "Enemy", distance);
					ImVec2 text_size = ImGui::CalcTextSize(distance_text);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(zero_w2s_r.x - (text_size.x / 2), zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), distance_text);
					bottom_text_offset += 14;
				}
				else if (this_player.bone_count = 103)
				{
					float distance = camera::location.distance(zero) / 100.f;
					char distance_text[256];
					sprintf_s(distance_text, "Female", distance);
					ImVec2 text_size = ImGui::CalcTextSize(distance_text);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(zero_w2s_r.x - (text_size.x / 2), zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), distance_text);
					bottom_text_offset += 14;
				}
				else if (this_player.bone_count = 105)
				{
					float distance = camera::location.distance(zero) / 100.f;
					char distance_text[256];
					sprintf_s(distance_text, "Male", distance);
					ImVec2 text_size = ImGui::CalcTextSize(distance_text);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(zero_w2s_r.x - (text_size.x / 2), zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), distance_text);
					bottom_text_offset += 14;
				}
			}

			ImGuiIO& io = ImGui::GetIO();
			ImFont* espfont = io.Fonts->AddFontFromFileTTF("C:\Windows\Fonts\Calibri.ttf", 14);
			ImGui::PushFont(espfont);
			ImGui::PopFont();



			if (config.player_box)
			{
				if (config.esptype == 0)
				{
					float BoxHeight = zero_w2s_r.y - head_w2s_r.y;
					float BoxWidth = BoxHeight / 2.f;
					draw_corner_box(zero_w2s_r.x - (BoxWidth / 2), head_w2s_r.y,
						BoxWidth, BoxHeight, config.espcolor, 2.f);
				}

				if (config.esptype == 1)
				{
					DrawNormalBox(zero_w2s.x - (BoxWidth / 2), vHeadBoneOut2.y, BoxWidth, BoxHeight, 2, config.espcolor);
				}

				if (config.esptype == 2)
				{
					draw_3d_box(zero, fvector(head.x, head.y, head.z + 20), 43, config.espcolor, 1.f);
				}
			}

			if (config.player_skeleton)
			{
				draw_skeleton(this_player, ImVec4(config.skeleton), (float)skeletonfloat);
			}


			if (config.player_agent)
			{
				int id = SEXread<int>(this_player.actor + 0x18);
				std::string agent_name = get_agent_name_by_id(id);
				ImVec2 text_size = ImGui::CalcTextSize(agent_name.c_str());

				ImGui::GetOverlayDrawList()->AddText(ImVec2(zero_w2s_r.x - (text_size.x / 2), zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), agent_name.c_str());
				bottom_text_offset += 14;
			}

			if (config.player_weapon)
			{
				uintptr_t inventory = SEXread<uintptr_t>(this_player.actor + 0x948);
				uintptr_t current_equip = SEXread<uintptr_t>(inventory + 0x218);
				int weapon_id = SEXread<int>(current_equip + 0x18);
				std::string weapon_name = get_weapon_name_by_id(weapon_id);
				ImVec2 text_size = ImGui::CalcTextSize(weapon_name.c_str());

				ImGui::GetOverlayDrawList()->AddText(ImVec2(zero_w2s_r.x - (text_size.x / 2), zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), weapon_name.c_str());
				bottom_text_offset += 14;
			}

			if (config.player_distance)
			{
				float distance = camera::location.distance(zero) / 100.f;
				char distance_text[256];
				sprintf_s(distance_text, "%.fm", distance);
				ImVec2 text_size = ImGui::CalcTextSize(distance_text);
				ImGui::GetOverlayDrawList()->AddText(ImVec2(zero_w2s_r.x - (text_size.x / 2), zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), distance_text);
				bottom_text_offset += 14;
			}

			if (config.player_snapline)
			{
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(center_x, center_y * 2), ImVec2(zero_w2s_r.x, zero_w2s_r.y + bottom_text_offset), ImGui::GetColorU32(config.snapcolor), 1.f);
			}



			if (config.player_view_angle)
			{
				fvector view_angle = SEXread<fvector>(this_player.root_component + 0x170);

				fvector test2, headpos;

				headpos = head;

				angle_rotation(view_angle, &test2);
				test2.x *= 260;
				test2.y *= 260;
				test2.z *= 260;

				fvector end = headpos + test2;
				fvector test1, test3;
				test1 = w2s(headpos);
				test3 = w2s(end);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(test1.x, test1.y), ImVec2(test3.x, test3.y), ImGui::GetColorU32({ 0.0f, 1.0f, 0.0f, 1.0f }), 1.f);
				draw_3d_box(end, fvector(end.x, end.y, end.z + 20), 30, ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 1.f);
			}

			float delta_x = head_w2s.x - (Width / 2.f);
			float delta_y = head_w2s.y - (Height / 2.f);
			float dist = sqrtf(delta_x * delta_x + delta_y * delta_y);
			float fovdist = CalculateDistance(Width / 2, Height / 2, head_w2s.x, head_w2s.y);
			if (visible && (dist < closest_distance) && fovdist < config.aimbot_fov) {
				closest_distance = dist;
				closestplayer = x;
			}

		}

		if (config.aimenable && !config.rcs && closestplayer != 1337)
		{
			player this_player = player_pawns[closestplayer];
			fvector head = bone_matrix(8, this_player);
			fvector chest = bone_matrix(6, this_player);
			fvector pelvis = bone_matrix(3, this_player);
			fvector bone;

			if (config.aimboness == 0)
			{
				bone = head;
			}

			if (config.aimboness == 1)
			{
				bone = chest;
			}

			if (config.aimboness == 2)
			{
				bone = pelvis;
			}

			fvector rootpos = SEXread<fvector>(this_player.root_component + 0x164);

			if (bone.z <= rootpos.z)
			{
				return;
			}

			fvector localView = SEXread<fvector>(pointer::player_controller + 0x440);
			fvector vecCaclculatedAngles = fhgfsdhkfshdghfsd205(camera::location, bone);
			fvector angleEx = CaadadalcAngle(camera::location, bone);
			fvector fin = fvector(vecCaclculatedAngles.y, angleEx.y, 0);
			fvector delta = fin - localView;
			NormalizeAngles(delta);

			fvector TargetAngle = localView + (delta / config.aimbot_smooth);
			fvector TargetAngle2 = localView + delta;
			NormalizeAngles(TargetAngle);
			NormalizeAngles(TargetAngle2);
			//Clamp(TargetAngle2);


			if (GetAsyncKeyState(AimKeyList[config.aimbotkey]) & 0x8000)
			{
				if (config.visiblecheck == true)
				{
					if (IsVisible)
					{
						SEXwrite<fvector>(pointer::player_controller + 0x440, TargetAngle);

					}
				}
				else
				{
					SEXwrite<fvector>(pointer::player_controller + 0x440, TargetAngle);

				}

			}
		}
		if (config.aimenable && config.rcs && closestplayer != 1337)
		{
			player this_player = player_pawns[closestplayer];
			fvector head = bone_matrix(8, this_player);
			fvector chest = bone_matrix(6, this_player);
			fvector pelvis = bone_matrix(3, this_player);
			fvector bone;

			if (config.aimboness == 0)
			{
				bone = head;
			}

			if (config.aimboness == 1)
			{
				bone = chest;
			}

			if (config.aimboness == 2)
			{
				bone = pelvis;
			}

			fvector rootpos = SEXread<fvector>(this_player.root_component + 0x164);

			if (bone.z <= rootpos.z)
			{
				return;
			}

			fvector localView = SEXread<fvector>(pointer::player_controller + 0x440);
			fvector vecCaclculatedAngles = fhgfsdhkfshdghfsd205(camera::location, bone);
			fvector angleEx = CaadadalcAngle(camera::location, bone);
			fvector fin = fvector(vecCaclculatedAngles.y, angleEx.y, 0);
			fvector delta = fin - localView;
			fvector TargetAngle2 = localView + delta;
			Clamp(TargetAngle2);


			if (GetAsyncKeyState(AimKeyList[config.aimbotkey]) & 0x8000)
			{
				if (config.visiblecheck == true)
				{
					if (IsVisible)
					{
						RCS(TargetAngle2, camera::rotation, config.aimbot_smooth);
					}
				}
				else
				{
					RCS(TargetAngle2, camera::rotation, config.aimbot_smooth);

				}


			}
			float temp = GetFov(camera::rotation, vecCaclculatedAngles);

		}

		if (config.aimbot_mode_silent == true && closestplayer != 1337)
		{
			player this_player = player_pawns[closestplayer];
			fvector head = bone_matrix(8, this_player);
			fvector chest = bone_matrix(6, this_player);
			fvector pelvis = bone_matrix(3, this_player);
			fvector bone;

			if (config.bonees == 0)
			{
				bone = head;
			}

			if (config.bonees == 1)
			{
				bone = chest;
			}

			if (config.bonees == 2)
			{
				bone = pelvis;
			}

			fvector rootpos = SEXread<fvector>(this_player.root_component + 0x164);

			if (bone.z <= rootpos.z)
			{
				return;
			}

			fvector localView = SEXread<fvector>(pointer::player_controller + 0x440);
			fvector vecCaclculatedAngles = fhgfsdhkfshdghfsd205(camera::location, bone);
			fvector angleEx = CaadadalcAngle(camera::location, bone);
			fvector fin = fvector(vecCaclculatedAngles.y, angleEx.y, 0);
			fvector delta = fin - localView;
			NormalizeAngles(delta);
			fvector TargetAngle = localView + (delta / config.silent_smooth);
			NormalizeAngles(TargetAngle);

			fvector original = SEXread<fvector>(pointer::player_controller + 0x440);

			if (GetAsyncKeyState(AimKeyList[silentKey]) & 0x8000)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				SEXwrite<fvector>(pointer::player_controller + 0x440, TargetAngle);
				Sleep(4);
				mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				Sleep(8);
				SEXwrite<fvector>(pointer::player_controller + 0x440, original);

			}
			float temp = GetFov(camera::rotation, vecCaclculatedAngles);
		}

		if (config.aimbot_draw_fov)
		{
			ImGui::GetOverlayDrawList()->AddCircle(ImVec2(center_x, center_y), config.aimbot_fov, ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), 64, 1.f);
		}



	}



	static HWND Window = NULL;
	IDirect3D9Ex* p_Object = NULL;
	static LPDIRECT3DDEVICE9 D3dDevice = NULL;
	static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
	HWND hwnd = NULL;
	RECT GameRect = { NULL };
	D3DPRESENT_PARAMETERS d3dpp;
	const MARGINS Margin = { -1 };

	DWORD ScreenCenterX = 960;
	DWORD ScreenCenterY = 540;
	MSG Message = { NULL };

	bool menu_open = true;
	bool first_time = true;

	bool imguiStarted = false;

	void render() {

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (GetAsyncKeyState(VK_INSERT) & 1)
			menu_open = !menu_open;

		sextheme();
		if (menu_open)
		{
			ImGui::GetIO().MouseDrawCursor = 1;
			if (ImGui::Begin("Nightware.pw", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings))
			{
				TssakliMenu();
			}ImGui::End();
		}
		else
			ImGui::GetIO().MouseDrawCursor = 0;
		auto isFrames2 = ImGui::GetFrameCount();

		static float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;

		ImVec4 isrgb = ImVec4(isRed, isGreen, isBlue, 1.0f);



		if (isFrames2 % 1 == 0) //We use modulus to check if it's divisible by 1, and if the remainder equals 0, then we continue. This effect gets called every frame.
		{

			if (isGreen == 0.01f && isBlue == 0.0f)
			{
				isRed += 0.01f;

			}

			if (isRed > 0.99f && isBlue == 0.0f)
			{
				isRed = 1.0f;

				isGreen += 0.01f;

			}

			if (isGreen > 0.99f && isBlue == 0.0f)
			{
				isGreen = 1.0f;

				isRed -= 0.01f;

			}

			if (isRed < 0.01f && isGreen == 1.0f)
			{
				isRed = 0.0f;

				isBlue += 0.01f;

			}

			if (isBlue > 0.99f && isRed == 0.0f)
			{
				isBlue = 1.0f;

				isGreen -= 0.01f;

			}

			if (isGreen < 0.01f && isBlue == 1.0f)
			{
				isGreen = 0.0f;

				isRed += 0.01f;

			}

			if (isRed > 0.99f && isGreen == 0.0f)
			{
				isRed = 1.0f;

				isBlue -= 0.01f;

			}

			if (isBlue < 0.01f && isGreen == 0.0f)
			{
				isBlue = 0.0f;

				isRed -= 0.01f;

				if (isRed < 0.01f)
					isGreen = 0.01f;

			}

		}


		static int maintabs = 0;

		render_players();




		D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

		if (D3dDevice->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			D3dDevice->EndScene();
		}
		HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

		if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			D3dDevice->Reset(&d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}

	bool start_directx() {
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
			return false;

		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferWidth = Width;
		d3dpp.BackBufferHeight = Height;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.hDeviceWindow = Window;
		d3dpp.Windowed = TRUE;

		p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);

		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		//fonts::standard_font = io.Fonts->AddFontDefault();
		//fonts::intro_font = io.Fonts->AddFontFromFileTTF("adfg.ttf", 300.f);
		//fonts::standard_font = io.Fonts->AddFontDefault();

		ImGui_ImplWin32_Init(Window);
		ImGui_ImplDX9_Init(D3dDevice);

		p_Object->Release();
		return true;
	}

	void wait_for_window() {
		while (true) {
			HWND foreground_window = GetForegroundWindow();
			HWND target_window = FindWindowA(0, "VALORANT  ");

			if (foreground_window == target_window)
				break;

			Sleep(200);
		}
	}

	void render_loop() {
		static RECT old_rc;
		ZeroMemory(&Message, sizeof(MSG));

		while (Message.message != WM_QUIT)
		{
			if (PeekMessage(&Message, Window, 0, 0, 0x0001))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}

			HWND hwnd_active = GetForegroundWindow();

			if (hwnd_active == hwnd) {
				HWND hwndtest = GetWindow(hwnd_active, 3);
				SetWindowPos(Window, hwndtest, 2, 2, -3, -3, 0x0002 | 0x0001);
			}

			RECT rc;
			POINT xy;

			ZeroMemory(&rc, sizeof(RECT));
			ZeroMemory(&xy, sizeof(POINT));
			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, &xy);
			rc.left = xy.x;
			rc.top = xy.y;

			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = hwnd;
			io.DeltaTime = 1.0f / 60.0f;

			POINT p;
			GetCursorPos(&p);
			io.MousePos.x = p.x - xy.x;
			io.MousePos.y = p.y - xy.y;

			if (GetAsyncKeyState(VK_LBUTTON)) {
				io.MouseDown[0] = true;
				io.MouseClicked[0] = true;
				io.MouseClickedPos[0].x = io.MousePos.x;
				io.MouseClickedPos[0].x = io.MousePos.y;
			}
			else
				io.MouseDown[0] = false;

			if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
			{
				old_rc = rc;

				Width = rc.right;
				Height = rc.bottom;

				d3dpp.BackBufferWidth = Width;
				d3dpp.BackBufferHeight = Height;
				SetWindowPos(Window, (HWND)0, xy.x + 2, xy.y + 2, Width - 3, Height - 3, 0x0008);
				D3dDevice->Reset(&d3dpp);
			}

			render();

		}
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		DestroyWindow(Window);
	}
	void stop_render() {
		TriBuf->Release();
		D3dDevice->Release();
		p_Object->Release();

		DestroyWindow(Window);
		UnregisterClassW((_(L"qwqlwqkwkqw")), NULL);
	}

	LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
			return true;

		switch (Message)
		{
		case WM_DESTROY:
			stop_render();
			PostQuitMessage(0);
			exit(4);
			break;
		case WM_SIZE:
			if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
				d3dpp.BackBufferWidth = LOWORD(lParam);
				d3dpp.BackBufferHeight = HIWORD(lParam);
				HRESULT hr = D3dDevice->Reset(&d3dpp);
				if (hr == D3DERR_INVALIDCALL)
					IM_ASSERT(0);
				ImGui_ImplDX9_CreateDeviceObjects();
			}
			break;
		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
		}
		return 0;
	}

	void create_window() {
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.lpszClassName = (_(L"qwqlwqkwkqw"));
		wc.lpfnWndProc = WinProc;
		RegisterClassEx(&wc);

		if (hwnd)
		{
			GetClientRect(hwnd, &GameRect);
			POINT xy;
			ClientToScreen(hwnd, &xy);
			GameRect.left = xy.x;
			GameRect.top = xy.y;

			Width = GameRect.right;
			Height = GameRect.bottom;
		}
		else
			exit(2);

		Window = CreateWindowExW(NULL, _(L"qwqlwqkwkqw"), _(L"qwqlwqkwkqw"), 0x80000000L | 0x10000000L, 2, 2, Width - 2, Height - 2, 0, 0, 0, 0);

		DwmExtendFrameIntoClientArea(Window, &Margin);
		SetWindowLong(Window, (-20), 0x00000020L | 0x00000080L | 0x00080000);

		ShowWindow(Window, SW_SHOW);
		UpdateWindow(Window);
	}

	void start_cheat() {
		hwnd = FindWindowA(0, _("VALORANT  ")); //window name

		create_window();

		start_directx();

		wait_for_window();

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)find, NULL, NULL, NULL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)calculate_random_for_constant_aimbot, NULL, NULL, NULL);

		render_loop();
		stop_render();
	}
}
