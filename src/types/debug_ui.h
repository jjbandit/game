
#define DEBUG_MAX_UI_WINDOW_SLICES 1024.0f
#define DISABLE_CLIPPING V2(f32_MAX)

#define DEBUG_UI_OUTLINE_VALUES  0
#define DEBUG_UI_OUTLINE_BUTTONS 1
#define DEBUG_UI_OUTLINE_TABLES  1


/******************************               ********************************/
/******************************   Rendering   ********************************/
/******************************               ********************************/




struct layout
{
  v2 Basis;
  v2 At;
  rect2 DrawBounds;
};

struct window_layout
{
  counted_string Title;

  v2 Basis;
  v2 MaxClip;

  u64 InteractionStackIndex;

  r32 zBackground;
  r32 zText;
  r32 zBorder;
  r32 zTitleBar;

  window_layout* NextHotWindow;
};



/***************************                **********************************/
/*************************** Command Buffer **********************************/
/***************************                **********************************/



enum debug_texture_array_slice
{
  DebugTextureArraySlice_Font,
  DebugTextureArraySlice_Viewport,

  DebugTextureArraySlice_Count,
};

enum column_render_params
{
  ColumnRenderParam_LeftAlign  = 0,
  ColumnRenderParam_RightAlign = (1 << 0),
};

enum quad_render_params
{
  QuadRenderParam_NoAdvance     =  0,
  QuadRenderParam_AdvanceLayout = (1 << 0),
  QuadRenderParam_AdvanceClip   = (1 << 1),

  QuadRenderParam_Default = (QuadRenderParam_AdvanceLayout|QuadRenderParam_AdvanceClip),
};

enum button_end_params
{
  ButtonEndParam_NoOp                    = 0,
  ButtonEndParam_DiscardButtonDrawBounds = (1 << 0),
};

enum relative_position
{
  Position_None,

  Position_LeftOf,
  Position_RightOf,
  Position_Above,
  Position_Below,
};

enum z_depth
{
  zDepth_Background,
  zDepth_Text,
  zDepth_TitleBar,
  zDepth_Border,
};

enum ui_render_command_type
{
  type_ui_render_command_noop,

  type_ui_render_command_window_start,
  type_ui_render_command_window_end,

  type_ui_render_command_table_start,
  type_ui_render_command_table_end,

  type_ui_render_command_button_start,
  type_ui_render_command_button_end,

  type_ui_render_command_text,
  type_ui_render_command_text_at,

  type_ui_render_command_column,
  type_ui_render_command_new_row,
  type_ui_render_command_border,

  type_ui_render_command_textured_quad,
  type_ui_render_command_untextured_quad,
  type_ui_render_command_untextured_quad_at,
};

enum ui_render_command_flags
{
  UiElement_Default,
  UiElement_Floating,
};









struct font
{
  v2 Size;
};

// TODO(Jesse): Axe this!
debug_global font Global_Font = {
  .Size = V2(30, 34),
};

struct ui_element_reference
{
  u32 Index;
};

struct ui_style
{
  v3 Color;

  v3 HoverColor;
  v3 PressedColor;
  v3 ClickedColor;
  v3 ActiveColor;

  v4 Padding;

  font Font;

  b32 IsActive;
};








struct ui_render_command_border
{
  window_layout* Window;
  rect2 Bounds;
  v3 Color;
};

struct ui_render_command_window_start
{
  layout Layout;
  window_layout* Window;
};

struct ui_render_command_window_end
{
  window_layout* Window;
};

struct ui_render_command_column
{
  b32 OverrideStyling;
  ui_style Style;

  column_render_params Params;
};

struct ui_render_command_text
{
  layout Layout;
  counted_string String;
};

struct ui_render_command_text_at
{
  counted_string Text;
  v2 At;
  v2 MaxClip;
};

struct ui_render_command_untextured_quad
{
  v2 OffsetFromLayout;
  v2 QuadDim;
  ui_style Style;
  z_depth zDepth;
  quad_render_params Params;
};

struct ui_render_command_untextured_quad_at
{
  layout Layout;
  v2 StartingAt;
  v2 QuadDim;
  ui_style Style;
  z_depth zDepth;
  quad_render_params Params;
};

struct ui_render_command_textured_quad
{
  debug_texture_array_slice TextureSlice;
  z_depth zDepth;
};

struct ui_render_command_button_start
{
  umm ID;
  ui_style Style;
};

struct ui_render_command_button_end
{
  button_end_params Params;
};

struct ui_render_command_table_start
{
  layout Layout;

  relative_position Position;
  ui_element_reference RelativeTo;
};







struct ui_render_command
{
  ui_render_command_type Type;
  ui_render_command_flags Flags;

  union
  {
    ui_render_command_window_start       ui_render_command_window_start;
    ui_render_command_window_end         ui_render_command_window_end;

    ui_render_command_button_start       ui_render_command_button_start;
    ui_render_command_button_end         ui_render_command_button_end;

    ui_render_command_table_start        ui_render_command_table_start;

    ui_render_command_text               ui_render_command_text;
    ui_render_command_text_at            ui_render_command_text_at;

    ui_render_command_textured_quad      ui_render_command_textured_quad;
    ui_render_command_untextured_quad    ui_render_command_untextured_quad;
    ui_render_command_untextured_quad_at ui_render_command_untextured_quad_at;

    ui_render_command_border             ui_render_command_border;
    ui_render_command_column             ui_render_command_column;
  };
};







struct table_render_params
{
  u32 *ColumnWidths;
  u16 ColumnCount;
  u16 CurrentColumn;

  u32 TableStart;
  u32 OnePastTableEnd;
};







#define MAX_UI_RENDER_COMMAND_COUNT (1024*1024)
struct ui_render_command_buffer
{
  u32 CommandCount;
  ui_render_command Commands[MAX_UI_RENDER_COMMAND_COUNT];
};







/******************************               ********************************/
/******************************      Misc     ********************************/
/******************************               ********************************/



enum clip_status
{
  ClipStatus_NoClipping,
  ClipStatus_PartialClipping,
  ClipStatus_FullyClipped
};

struct clip_result
{
  clip_status ClipStatus;
  v2 MaxClip;

  rect2 PartialClip;
};

struct sort_key
{
  u64 Value;
  u32 Index;
};

struct window_sort_params
{
  u32 Count;
  u64 LowestInteractionStackIndex;

  sort_key* SortKeys;
};







#define DEBUG_FONT_SHADOW_EPSILON (0.00001f)

function r32
GetZ(z_depth zDepth, window_layout* Window)
{
  r32 Result = DEBUG_FONT_SHADOW_EPSILON;

  if (Window)
  {
    switch (zDepth)
    {
      case zDepth_Background:
      {
        Result = Window->zBackground;
      } break;
      case zDepth_Text:
      {
        Result = Window->zText;
      } break;
      case zDepth_TitleBar:
      {
        Result = Window->zTitleBar;
      } break;
      case zDepth_Border:
      {
        Result = Window->zBorder;
      } break;

      InvalidDefaultCase;
    }
  }

  return Result;
}

function ui_style
StandardStyling(v3 StartingColor, v3 HoverMultiplier = V3(1.3f), v3 ClickMultiplier = V3(1.2f))
{
  ui_style Result = {};
  Result.Color = StartingColor;
  Result.HoverColor = StartingColor*HoverMultiplier;
  Result.ClickedColor = StartingColor*ClickMultiplier;

  return Result;
}

function ui_style
UiStyleFromLightestColor(v3 Color, v4 Padding = V4())
{
  ui_style Style  = {
    .Color        = Color,
    .HoverColor   = Color*0.75f,
    .PressedColor = Color,
    .ClickedColor = Color,
    .ActiveColor  = Color,

    .Font         = Global_Font,

    .Padding      = Padding,
    .IsActive     = False,
  };

  return Style;
}

function ui_style
UiStyleFromLightestColor(v3 Color, v2 Padding)
{
  ui_style Style = UiStyleFromLightestColor(Color, V4(Padding.x, Padding.y, Padding.x, Padding.y));
  return Style;
}


function window_layout
WindowLayout(const char* Title, v2 Basis, v2 MaxClip = V2(1800, 800))
{
  window_layout Window = {};
  Window.Basis = Basis;
  Window.MaxClip = MaxClip;
  Window.Title = CS(Title);

  return Window;
}

v2 GetAbsoluteMaxClip(window_layout* Window);

function rect2
GetWindowBounds(window_layout *Window)
{
  v2 TopLeft = Window->Basis;
  v2 BottomRight = GetAbsoluteMaxClip(Window);
  rect2 Result = RectMinMax(TopLeft, BottomRight);
  return Result;
}
