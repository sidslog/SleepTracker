/*
 * pebble pedometer
 * @author jathusant
 */

#include <pebble.h>
#include <run.h>
#include "value_holder.h"
#include "persist.h"
#include <pedometer.h>

void update_ui_callback(int value);
void settings_load(Window *window);
void settings_unload(Window *window);

static Window *window;
static Window *menu_window;
static Window *pedometer;

static SimpleMenuLayer *pedometer_settings;
static SimpleMenuItem menu_items[2];
static SimpleMenuSection menu_sections[1];
ActionBarLayer *stepGoalSetter;

// Menu Item names and subtitles
char *item_names[2] = { "Start sleep", "Start fit"};
char *item_sub[2] = { "Lets gor for a sleep!", "Lets exercise"};

// Text Layers
TextLayer *main_message;
TextLayer *main_message2;
TextLayer *hitBack;
TextLayer *stepGoalVisualizer;
TextLayer *steps;
TextLayer *pedCount;
TextLayer *infor;
TextLayer *calories;
TextLayer *stepGoalText;

// Bitmap Layers
static GBitmap *btn_dwn;
static GBitmap *btn_up;
static GBitmap *btn_sel;
static GBitmap *statusBar;
GBitmap *pedometerBack;
BitmapLayer *pedometerBack_layer;
GBitmap *splash;
BitmapLayer *splash_layer;

// values for max/min number of calibration options 
const int MAX_CALIBRATION_SETTINGS = 3;
const int MIN_CALIBRATION_SETTINGS = 1;

// steps required per calorie
const int STEPS_PER_CALORIE = 22;

time_t lastUpdateTime = 0;
uint32_t sleepCounter = 0;

void ped_load(Window *window) {
    steps = text_layer_create(GRect(0, 20, 150, 170));
    pedCount = text_layer_create(GRect(0, 80, 150, 170));
    calories = text_layer_create(GRect(0, 10, 150, 170));
    
    window_set_background_color(pedometer, GColorBlack);
    pedometerBack = gbitmap_create_with_resource(RESOURCE_ID_PED_WHITE);
    text_layer_set_background_color(steps, GColorClear);
    text_layer_set_text_color(steps, GColorBlack);
    text_layer_set_background_color(pedCount, GColorClear);
    text_layer_set_text_color(pedCount, GColorBlack);
    text_layer_set_background_color(calories, GColorClear);
    text_layer_set_text_color(calories, GColorWhite);
    
    pedometerBack_layer = bitmap_layer_create(GRect(0, 0, 145, 185));
    
    bitmap_layer_set_bitmap(pedometerBack_layer, pedometerBack);
    layer_add_child(window_get_root_layer(pedometer),
                    bitmap_layer_get_layer(pedometerBack_layer));
    
    layer_add_child(window_get_root_layer(pedometer), (Layer*) steps);
    layer_add_child(window_get_root_layer(pedometer), (Layer*) pedCount);
    layer_add_child(window_get_root_layer(pedometer), (Layer*) calories);
    
    text_layer_set_font(pedCount,
                        fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_font(calories,
                        fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    
    text_layer_set_text_alignment(pedCount, GTextAlignmentCenter);
    text_layer_set_text_alignment(calories, GTextAlignmentCenter);
    
    text_layer_set_text(steps, "\n\n\n                S T E P S");
    
    uint32_t fit = addFitValue(0);
    
    static char buf[] = "1234567890";
    snprintf(buf, sizeof(buf), "%ld", (long)fit);
    text_layer_set_text(pedCount, buf);
    
    static char buf2[] = "1234567890abcdefghijkl";
    snprintf(buf2, sizeof(buf2), "%ld Calories", (long) fit/STEPS_PER_CALORIE);
    text_layer_set_text(calories, buf2);
}

void ped_unload(Window *window) {
    stopTrack();
    window_destroy(pedometer);
    text_layer_destroy(pedCount);
    text_layer_destroy(calories);
    text_layer_destroy(steps);
    gbitmap_destroy(pedometerBack);
}



void start_sleep_callback(int index, void *ctx) {
	accel_data_service_subscribe(0, NULL);

	menu_items[0].title = "Continue Run";
	menu_items[0].subtitle = "Ready for more?";
	layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));

    setMode(MODE_SLEEP);
    
	pedometer = window_create();

	window_set_window_handlers(pedometer, (WindowHandlers ) { .load = ped_load,
					.unload = ped_unload, });

	window_stack_push(pedometer, true);

    lastUpdateTime = time(NULL);
    sleepCounter = 0;
    startTrack(update_ui_callback);
}

void start_fit_callback(int index, void *ctx) {
    accel_data_service_subscribe(0, NULL);
    
    menu_items[0].title = "Continue Run";
    menu_items[0].subtitle = "Ready for more?";
    layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));
    
    setMode(MODE_FIT);
    
    pedometer = window_create();
    
    window_set_window_handlers(pedometer, (WindowHandlers ) { .load = ped_load,
        .unload = ped_unload, });
    
    window_stack_push(pedometer, true);
    
    startTrack(update_ui_callback);
}

void setup_menu_items() {

	for (int i = 0; i < (int) (sizeof(item_names) / sizeof(item_names[0]));
			i++) {
		menu_items[i] = (SimpleMenuItem ) { .title = item_names[i], .subtitle =
						item_sub[i], };

		//Setting Callbacks
		if (i == 0) {
			menu_items[i].callback = start_fit_callback;
		} else if (i == 1) {
			menu_items[i].callback = start_sleep_callback;
		}
	}
}

void setup_menu_sections() {
	menu_sections[0] = (SimpleMenuSection ) { .items = menu_items, .num_items =
					sizeof(menu_items) / sizeof(menu_items[0]) };
}

void setup_menu_window() {
	menu_window = window_create();

	window_set_window_handlers(menu_window, (WindowHandlers ) { .load =
					settings_load, .unload = settings_unload, });
}

void settings_load(Window *window) {
    Layer *layer = window_get_root_layer(menu_window);
    statusBar = gbitmap_create_with_resource(RESOURCE_ID_STATUS_BAR);
    
    pedometer_settings = simple_menu_layer_create(layer_get_bounds(layer),
                                                  menu_window, menu_sections, 1, NULL);
    simple_menu_layer_set_selected_index(pedometer_settings, 0, true);
    layer_add_child(layer, simple_menu_layer_get_layer(pedometer_settings));
    window_set_status_bar_icon(menu_window, statusBar);
}

void settings_unload(Window *window) {
    layer_destroy(window_get_root_layer(menu_window));
    simple_menu_layer_destroy(pedometer_settings);
    window_destroy(menu_window);
}

void window_load(Window *window) {

	splash = gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
	window_set_background_color(window, GColorBlack);

	splash_layer = bitmap_layer_create(GRect(0, 0, 145, 185));
	bitmap_layer_set_bitmap(splash_layer, splash);
	layer_add_child(window_get_root_layer(window),
			bitmap_layer_get_layer(splash_layer));

	main_message = text_layer_create(GRect(0, 0, 150, 170));
	main_message2 = text_layer_create(GRect(3, 30, 150, 170));
	hitBack = text_layer_create(GRect(3, 40, 200, 170));

	text_layer_set_background_color(main_message, GColorClear);
	text_layer_set_text_color(main_message, GColorWhite);
	text_layer_set_font(main_message,
			fonts_load_custom_font(
					resource_get_handle(RESOURCE_ID_ROBOTO_LT_30)));
	layer_add_child(window_get_root_layer(window), (Layer*) main_message);

	text_layer_set_background_color(main_message2, GColorClear);
	text_layer_set_text_color(main_message2, GColorWhite);
	text_layer_set_font(main_message2,
			fonts_load_custom_font(
					resource_get_handle(RESOURCE_ID_ROBOTO_LT_15)));
	layer_add_child(window_get_root_layer(window), (Layer*) main_message2);

	text_layer_set_background_color(hitBack, GColorClear);
	text_layer_set_text_color(hitBack, GColorWhite);
	text_layer_set_font(hitBack,
			fonts_load_custom_font(
					resource_get_handle(RESOURCE_ID_ROBOTO_LT_15)));
	layer_add_child(window_get_root_layer(window), (Layer*) hitBack);

	text_layer_set_text(main_message, "      Goal");
	text_layer_set_text(main_message2, "          Reached!");
	text_layer_set_text(hitBack, "\n\n\n\n\n\n     << Press Back");
}

void window_unload(Window *window) {
	window_destroy(window);
	text_layer_destroy(main_message);
	text_layer_destroy(main_message2);
	text_layer_destroy(hitBack);
	bitmap_layer_destroy(splash_layer);
}


void update_ui_callback(int value) {
    
    if (currentMode() == MODE_FIT) {
        long pedometerCount = addFitValue(1);
        
        long caloriesBurned = (int) (pedometerCount / STEPS_PER_CALORIE);
        static char calBuf[] = "123456890abcdefghijkl";
        snprintf(calBuf, sizeof(calBuf), "%ld Calories", caloriesBurned);
        text_layer_set_text(calories, calBuf);
        
        static char buf[] = "123456890abcdefghijkl";
        snprintf(buf, sizeof(buf), "%ld", pedometerCount);
        text_layer_set_text(pedCount, buf);
        
        
        layer_mark_dirty(window_get_root_layer(pedometer));
        layer_mark_dirty(window_get_root_layer(menu_window));
        layer_mark_dirty(window_get_root_layer(pedometer));
    } else {
        sleepCounter += value;
        time_t currentTime = time(NULL);
        if (lastUpdateTime > 0 && (currentTime - lastUpdateTime) > 60 * 10) {
            text_layer_set_text(calories, "");

            static char buf[] = "123456890abcdefghijkl";
            snprintf(buf, sizeof(buf), "%ld", (long) sleepCounter);
            text_layer_set_text(pedCount, buf);


            layer_mark_dirty(window_get_root_layer(pedometer));
            layer_mark_dirty(window_get_root_layer(menu_window));
            layer_mark_dirty(window_get_root_layer(pedometer));

            addSleepValue(sleepCounter);
            sleepCounter = 0;
        }
        
    }

}


void handle_init(void) {

	window = window_create();

	setup_menu_items();
	setup_menu_sections();
	setup_menu_window();

	window_stack_push(menu_window, true);
}

void handle_deinit(void) {
	accel_data_service_unsubscribe();
// 	window_destroy(menu_window);
}