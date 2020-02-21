/// for popups
typedef void (*fun)(class uitree*,const pointf2& popup_loc);
struct popup_info {
	const C8* butname;
	fun popup_fun;
};

/// rectangle class
extern const popup_info rect_new_class[];
/// bitmap class
extern const popup_info bitmap_change_color1[];
/// texter class
extern const popup_info texter_changename[];
/// button class
extern const popup_info button_changename[];
/// listbox2d class
extern const popup_info listbox2d_addstring[];
/// slider class
extern const popup_info slider_changeleftrightval[];
extern const popup_info slider_changetopbotval[];
