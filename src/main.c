#include <pebble.h>
#include "consts.h"
#include "weather.h"
  
weather_info daily_info[8];
weather_info hourly_info[48];

Window* window;
 
MenuLayer *menu_layer;

int type = 3;

void trigger_js (void) {
  APP_LOG (APP_LOG_LEVEL_WARNING,"Initiating connection");
 
  DictionaryIterator* iter;
    
  app_message_outbox_begin(&iter);
 
  Tuplet value = TupletInteger(KEY_TYPE,type);
  dict_write_tuplet(iter, &value);
 
  app_message_outbox_send();
  
  APP_LOG (APP_LOG_LEVEL_WARNING,"Done connection");

}

static void init_daily_info () {
  
  int i = 0;
  
  for (i=0;i<8;i++) {
    //APP_LOG (APP_LOG_LEVEL_INFO, "Init daily_info[%u]",i);
    daily_info[i].conditions = 0;
    daily_info[i].time= 0;
    strcpy (daily_info[i].summary,"Requesting data from phone");
    daily_info[i].temperature= 0;
    daily_info[i].feelslike= 0;
    daily_info[i].wind= 0;
    daily_info[i].winddir= 0;
  }
  
  for (i=0;i<48;i++) {
    hourly_info[i].conditions = 0;
    hourly_info[i].time= 0;
    strcpy (hourly_info[i].summary,"Requesting data from phone");
    hourly_info[i].temperature= 0;
    hourly_info[i].feelslike= 0;
    hourly_info[i].wind= 0;
    hourly_info[i].winddir= 0;  
  }
  
}

static void daily_received_handler(DictionaryIterator *iter, void *context)
{
  int shift=0;

  int conditions=0;
  time_t time=0;
  char summary[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  uint8_t temperature = 0;
  uint8_t feelslike = 0;
  uint8_t wind = 0;
  uint16_t winddir = 0;
  // Read first item

  Tuple *t = dict_read_first(iter);
  
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
      case KEY_SHIFT:
        shift = (int)t->value->int32;
      break;
      case KEY_TIME:
        time = (int)t->value->int32;      
      break;
      case KEY_CONDITIONS:
        conditions = (int)t->value->int32;
      break;
      case KEY_SUMMARY:
        strcpy (summary,t->value->cstring);
      break;
      case KEY_TEMPERATURE:
        temperature = (int)t->value->uint8;
      break;
      case KEY_FEELSLIKE:
        feelslike = (int)t->value->uint8;
      break;
      case KEY_WIND:
        wind = (int)t->value->uint8;
      break;
      case KEY_WINDDIR:
        winddir = (int)t->value->uint16;
      break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
    // Look for next item
    t = dict_read_next(iter);
  }
  switch (type) {
    
  case 1:
    hourly_info[shift].time = time;
    strcpy (hourly_info[shift].summary,summary);
    hourly_info[shift].temperature = temperature;
    hourly_info[shift].conditions = conditions;
  
    hourly_info[shift].feelslike = feelslike;
    hourly_info[shift].wind = wind;
    hourly_info[shift].winddir = winddir;
  break;
    
  case 2:
  daily_info[shift].time = time;
  strcpy (daily_info[shift].summary,summary);
  daily_info[shift].temperature = temperature;
  daily_info[shift].conditions = conditions;
  
  daily_info[shift].feelslike = feelslike;
  daily_info[shift].wind = wind;
  daily_info[shift].winddir = winddir;
  break;
  } 
  //trigger a full reload of the menu
  
  menu_layer_reload_data (menu_layer);
  
}


void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
  int row; 
  
//  static char title [] =   "Wednesday 31";
  static char title [] =   "Loading data";

  // static char loading [] = "Loading data";
  //static char content[64];
  
  struct tm *tick_time;
  
 row = cell_index->row;
 
  switch (type) {
    case 0:
      APP_LOG (APP_LOG_LEVEL_INFO, "Nothing to do here");
    break;
    case 1:
      APP_LOG (APP_LOG_LEVEL_INFO, "Entering daily_draw_row_callback %u", row);
      if (hourly_info[row].time > 0) {
        tick_time = localtime (&(hourly_info[row].time));    
        strftime(title, sizeof ("Loading data from phone"), "%A %d %H:00", tick_time);
      } 
      menu_cell_basic_draw(ctx, cell_layer, title, hourly_info[row].summary, NULL);
    break;
    case 2:
      APP_LOG (APP_LOG_LEVEL_INFO, "Entering daily_draw_row_callback %u", row);
      if (daily_info[row].time > 0) {
        tick_time = localtime (&(daily_info[row].time));
        strftime(title, sizeof ("Loading data"), "%A %d", tick_time);
      } 
      menu_cell_basic_draw(ctx, cell_layer, title, daily_info[row].summary, NULL);
    break;
    case 3:
      switch(cell_index->row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "Current conditions", "", NULL);
        break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Hourly forecast", "for the next 48 hours", NULL);
        break;
        case 2:
          menu_cell_basic_draw(ctx, cell_layer, "Daily forecast", "for the next 7 days", NULL);
        break;
      }
    break;
  }
  
   
}

uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
  int returnvalue=3;
  
  switch (type) {
    case 0:
      returnvalue=1;
    break;
    case 1:
      returnvalue=48;
    break;
    case 2:
      returnvalue=8;
    break;
    case 3:
      returnvalue=3; // 3 menus items
    break;
  }
 return returnvalue;
}
 
void select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
   //Get which row
	int which = cell_index->row;
	
  switch (which) {
    case 0:
      APP_LOG (APP_LOG_LEVEL_INFO,"MENU ITEM 0 SELECTED");
    break;
    case 1:
      APP_LOG (APP_LOG_LEVEL_INFO,"MENU ITEM 1 SELECTED");
     // show_hourly ();
     type = 1;
    break;
    
    case 2:
      APP_LOG (APP_LOG_LEVEL_INFO,"MENU ITEM 2 SELECTED");
      // show_daily ();
      type = 2;
    break;
    
    default:
      APP_LOG (APP_LOG_LEVEL_ERROR,"ARE YOU KIDDING ME ?!");
    break;
  }
   trigger_js();
   menu_layer_reload_data (menu_layer);
 
}

void window_load(Window *window)
{
  init_daily_info ();
  
 //Create it - 12 is approx height of the top bar
	menu_layer = menu_layer_create(GRect(0, 0, 144, 168 - 16));
	
	//Let it receive clicks
	menu_layer_set_click_config_onto_window(menu_layer, window);
	
	//Give it its callbacks
	MenuLayerCallbacks callbacks = {
		.draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
		.get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
		.select_click = (MenuLayerSelectCallback) select_click_callback
	};
	menu_layer_set_callbacks(menu_layer, NULL, callbacks);
	
	//Add to Window
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
  
  
  app_message_register_inbox_received(daily_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());    //Largest possible input and output buffer sizes
 }
 
void window_unload(Window *window)
{
 
}

void init()
{
    window = window_create();
    WindowHandlers handlers = {
        .load = window_load,
        .unload = window_unload
    };
    window_set_window_handlers(window, (WindowHandlers) handlers);
    window_stack_push(window, true);
    trigger_js();
}
 
void deinit()
{
    window_destroy(window);
}
 
int main(void)
{
    init();
    app_event_loop();
    deinit();
}