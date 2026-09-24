# scui_obj_inf —— object 系列控件样式接口速查

> 提示文档：说明每个继承 obj_xxx 支持哪些样式(part/form/state/style)，以及本仓库实际用到的 prop。
> 对应代码：`scui/widget/object/`，样式入口 `scui_obj_inf.h`，绘制核心 `scui_object_sub.c`。

## 一、继承关系

```
scui_object_t(基类: 样式动画系统, 自动执行注册的tran)
│
├── scui_obj_btn    样式(def/pre/chk)  rect(bg部分: base/edge/box/sha)
│
├── scui_obj_arc    样式(def)          arc(bg/fg部分: base) + knob(端点)
│   └── scui_obj_spinner  样式(def)    arc(bg/fg部分: base)  ← 只加旋转事件
│
├── scui_obj_bar    样式(def)          rect(bg/fg部分: base/edge/box/sha)
│   ├── scui_obj_slider    ← 继承bar, 只加ptr跟手
│   └── scui_obj_switch    ← 继承bar, 只加click翻转
│
└── scui_obj_chart  样式(def)          rect(fg部分: base) + line(无层级)
```

- spinner/slider/switch 是解耦分离：**绘制核心在父类**，子类只加事件或定向参数。
- 因此父类样式扩充（如 bar 四件套、arc knob）对子类自动生效。

## 二、样式系统(part/form/state/style 四级编码)

属性存储：`(part, form, state, style) -> data`，同键原地覆盖（`scui_object_prop_add`）。
过渡存储：`(part, form, state_p, state_n, style) -> data_p/data_n`，样式动画系统自动推进（`scui_object_tran_add`）。

```c
type_part  = 0x1000   type_form = 0x2000   type_state = 0x3000   type_style = 0x4000   mask = 0x00FF
sub: 0x0000(main) 0x0100(sub1: 全局) 0x0200(sub2: rect) 0x0300(sub3: arc) 0x0400(sub4: line)
```

组合语义为 `type:<state, part, form, style>`：
- **part** 是"关键部分"（bg/fg/knob/item），决定绘制类型与样式套系；
- **form** 是"样式层级"（base/edge/box/sha），**每一个部分都有完整的层级套系**——bg 能画四层，fg 也能画四层，arc 也可以像 rect 一样绘制四个层级；
- **state/style** 体系不变。

### part 枚举（关键部分, scui_object.h）

| part | 值 | 含义 |
|---|---|---|
| main | 0x1001 | 全局(所有控件) |
| rect_bg | 0x1002 | 矩形背景(btn/bar 背景主体) |
| rect_fg | 0x1003 | 矩形前景(bar 进度 / chart 直方) |
| rect_knob | 0x1004 | 矩形端点(预留扩充) |
| line_item | 0x1005 | 线条部分(chart 折线) |
| arc_bg | 0x1006 | 弧形背景(arc/spinner 底弧) |
| arc_fg | 0x1007 | 弧形前景(动态进度弧) |
| arc_knob | 0x1008 | 弧形端点(arc 可选) |

### form 枚举（样式层级, scui_object.h）

| form | 值 | 用途 |
|---|---|---|
| rect_base | 0x2001 | 矩形基础(主体绘制) |
| rect_edge | 0x2002 | 矩形边界(可选扩充) |
| rect_box | 0x2003 | 矩形盒子(可选扩充) |
| rect_sha | 0x2004 | 矩形阴影(可选扩充) |
| arc_base | 0x2005 | 弧形基础(主体绘制) |
| arc_edge | 0x2006 | 弧形边界(可选扩充) |
| arc_box | 0x2007 | 弧形盒子(可选扩充) |
| arc_sha | 0x2008 | 弧形阴影(可选扩充) |

> 全局区属性(main_time)与 chart 线条通道 form 恒为 0。

### state 枚举

| state | 值 | 用途 |
|---|---|---|
| def | 0x3001 | 默认(所有控件) |
| pre | 0x3002 | 按下(仅 btn) |
| chk | 0x3003 | 选中(仅 btn check=1) |

### style 枚举（三系）

| rect 系(0x4201..0x420A) | 含义 |
|---|---|
| rect_alpha / rect_color / rect_point / rect_align | 透明度/色调/偏移点/对齐 |
| rect_width / rect_height | 宽/高 |
| rect_radius | 圆角(最大:<0) |
| rect_stroke | 边框(实心:<=0;空心:>0) |
| rect_grad_c | 渐变终点色 |
| rect_multi | multi: shadow/grad_w/grad |

| arc 系(0x4301..0x4309) | 含义 |
|---|---|
| arc_alpha / arc_color / arc_angle_s / arc_angle_e | 透明度/色调/起角/终角 |
| arc_center / arc_radius / arc_stroke | 弧心/半径/弧宽 |
| arc_grad_c | 渐变终点色 |
| arc_multi | multi: round/grad_w/grad |

| line 系(0x4401..0x4407) | 含义 |
|---|---|
| line_alpha / line_color / line_area | 透明度/色调/区域 |
| line_vpos / line_vpos_num | 端点序列/数量 |
| line_stroke / line_multi | 线宽/multi: round/grad |

| 全局(0x4102) | 含义 |
|---|---|
| main_time | 动画时间(ms), 值/角动画按跨度比例折算(全局区属性, form 恒为 0) |

## 三、各控件支持的样式(part/form)与常用 prop

### obj_btn —— 样式(def/pre/chk) · rect_bg × rect(base/edge/box/sha)

- **可 DIY 层级**：`rect_sha / rect_box / rect_edge / rect_base`（绘制顺序：阴影→盒子→边界→基础）
- **part**：固定 `rect_bg`
- **状态**：def(默认) / pre(按下) / chk(check=1 选中)；颜色+缩放 def↔pre / chk↔pre 过渡
- **实际用到的 prop**：
  - `rect_alpha`(cover) / `rect_color`(def/pre/chk 状态色) / `rect_grad_c`(渐变终点)
  - `rect_point`(未用, 0) / `rect_align`(居中 c)
  - `rect_width` / `rect_height`(fixed=1 固定 area; 否则按下缩放 lim/100)
  - `rect_radius` / `rect_stroke`(空心描边) / `rect_multi`(shadow/grad_w/grad)
  - `main_time`(动画时间)
- **tran 过渡**：color / grad_c / width / height（def↔pre, chk↔pre 双向）

### obj_arc / obj_spinner —— 样式(def) · arc_bg/arc_fg × arc_base + arc_knob

- **可 DIY 层级**：bg/fg 两部分各支持 `arc_base / arc_edge / arc_box / arc_sha`（当前绘制核心只用 base，其余层级预留给后续扩充）
- **part**：`arc_bg`(底弧) / `arc_fg`(动态进度弧) / `arc_knob`(端点)
- **状态**：仅 def（进度由 update_value/update_angle 驱动, 不走状态切换）
- **实际用到的 prop**：
  - `arc_alpha`(cover) / `arc_color`(bg[0]/fg[1]) / `arc_grad_c`(渐变终点)
  - `arc_angle_s` / `arc_angle_e`(fg 动态; spinner 每帧重写; 跟手经 update_angle)
  - `arc_center` / `arc_radius`(弧心/半径; knob 几何由自身 prop 配置)
  - `arc_stroke`(弧宽: 扇形<=0 / 弧型>0) / `arc_multi`(round/grad_w/grad)
  - `main_time`(动画时间, 按角度跨度折算)
- **knob 说明**：端点圆为**第三套独立样式**(part=`arc_knob`)，与 bg/fg 三套参数堆叠绘制(均走 `draw_arc` 原语)。
  - 几何/圆半径/描边/色调/圆角全部取 knob 自身 prop(center/radius/stroke/color 等, 由用户配置)
  - 颜色/透明度/round 用 knob 自身 res（color 取前景槽[1]）
- **tran 过渡**：fg 的 angle_s/angle_e（值/角动画, 单方向）

### obj_bar / obj_slider / obj_switch —— 样式(def) · rect_bg/rect_fg/rect_knob × rect(base/edge/box/sha)

- **可 DIY 层级**：bg 部分 `rect_sha / rect_box / rect_edge / rect_base`(静态四件套)；fg 部分 `rect_base`(进度条主体)
- **part**：`rect_bg`(背景) / `rect_fg`(前景进度) / `rect_knob`(端点圆, 可选)
- **状态**：仅 def（进度由 update_value 驱动）
- **实际用到的 prop**：
  - `rect_alpha`(cover) / `rect_color`(bg基础[0]/fg基础[1]/edge[2]/box[3]; sha 复用 box[3]; knob 用[1]) / `rect_grad_c`
  - `rect_align`(左上 l|u) / `rect_width` / `rect_height`(area 或 clip; fg 按 way 动态)
  - `rect_radius`(圆角; update_value 用 bg 半径反推 size_min) / `rect_stroke`(空心描边)
  - `rect_multi`(shadow/grad_w/grad)
  - `main_time`(动画时间, 按进度跨度折算)
- **fg 动态尺寸**：way=0 水平→宽动态; way=1 垂直→高动态；初始 0（update_value 前不显示）
- **knob 说明**：端点圆为**第三套独立样式**(part=`rect_knob`)，直径上限=fg 固定厚度(style 统一计算写入 prop)、**圆角由 UI 带入**(res->radius，默认同 fg，本地不运算)；**包围矩形动态裁剪**：宽/高=min(fg 当前尺寸, 直径)，左/上边缘贴 fg 增长端——放得下整圆时为 d×d 正方形(圆角钳制=圆)，放不下时与 fg 一致(圆角钳制自动退化)；update_value 与 fg 一样**只更新 w/h**(`rect_width/height`)并额外多更新一个 `rect_point`(端点坐标)，三者(含 fg 尺寸)与 fg **平行注册过渡**(同 time)，由过渡系统自动推进；直接更新走 `prop_add`；draw 并入 part_table 统一 `draw_rect` 从样式取；颜色用 fg 槽[1]；tran_sync 已为 `rect_point` 补线性插值(x/y 两维)
- **tran 过渡**：fg 的 width/height（值动画, 单方向）
- slider/switch 仅加事件：slider 拖动跟手(无动画), switch 点击翻转(端点动画); switch 默认启用 rect_knob

### obj_chart —— 样式(def) · rect_fg × rect_base + line_item(无层级)

- **可 DIY part/form**：`rect_fg × rect_base`(直方柱) / `line_item`(折线+渐变面积, form 恒 0)
- **状态**：仅 def
- **实际用到的 prop**：
  - rect_fg：`rect_width`(柱宽, draw 前动态读) / `rect_point` / `rect_height`(逐柱动态写)
  - line_item：`line_stroke`(线宽) / `line_vpos`(折线端点序列, 逐点动态写) / `line_multi`(round/grad)
  - `line_area`(区域) / `line_color` / `line_alpha`
- **无 tran**：chart 数据直接 prop_add 后绘制

## 四、res 结构字段说明（scui_obj_inf.h）

### scui_obj_btn_res_t
| 字段 | 说明 |
|---|---|
| part | 关键部分(固定 rect_bg) |
| form | 层级(base/edge/box/sha) |
| area | 部件宽高(0=控件clip) |
| color[4] | def[0]/pre[1]/chk[2]/pre[3]; s状态色→e渐变 |
| width | 边界(实心<=0 / 空心>0) |
| radius | 圆角(最大:<0) |
| time | 动画时间(ms, 0=默认 SCUI_WIDGET_OBJ_BTN_TIME) |
| lim | 按下缩放 pct(0=默认) |
| grad/gradw/shadow | 渐变/渐变方向/阴影 |

### scui_obj_arc_res_t
| 字段 | 说明 |
|---|---|
| part | 关键部分(arc_bg / arc_fg / arc_knob) |
| form | 层级(base; 可扩 edge/box/sha) |
| center | 弧心 |
| width | 弧宽(扇形<=0 / 弧型>0); knob 时=描边宽 |
| radius | 半径(>0); knob 时=端点圆半径 |
| angle_s/angle_e | 起/终角(默认 0/360; 容错: 相等时取 0/360) |
| color[2] | 背景[0]/前景[1](knob 用[1]) |
| time | 动画时间(ms, 0=默认, 按角度跨度折算) |
| round/gradw/grad | 端点圆角/渐变方向/渐变 |

### scui_obj_bar_res_t
| 字段 | 说明 |
|---|---|
| part | 关键部分(rect_bg / rect_fg / rect_knob) |
| form | 层级(base/edge/box/sha) |
| area | 部件宽高(0=控件clip) |
| color[4] | bg基础[0]/fg基础[1]/edge[2]/box[3](sha 复用[3]; knob 用[1]); s→e渐变 |
| width | 边界(实心<=0 / 空心>0) |
| radius | 圆角(最大:<0) |
| time | 动画时间(ms, 0=默认 SCUI_WIDGET_OBJ_BAR_TIME) |
| grad/gradw/shadow | 渐变/渐变方向/阴影 |

### scui_obj_chart_res_t
| 字段 | 说明 |
|---|---|
| part | 关键部分(line_item / rect_fg) |
| form | 层级(base; 线条恒 0) |
| round | 折线端点圆角 |
| grad | 折线渐变面积(曲线→底边) |
| color | 颜色 |
| width | 线宽 |

## 五、注意

- **缺 prop 静默跳过**：draw 时 `scui_object_draw_prop_sync` 缺属性返回 false（不绘制、不报错）；
  未配置的层级（如 bar 的 edge/box/sha）会自动跳过，因此四件套是"按需配置"。
- **knob 依赖 bg+fg**：必须同时配置 `arc_bg × arc_base`(几何) 与 `arc_fg × arc_base`(动态角)，knob 才有效。
- **fg 进度复位**：bar 的 style 只在 `rect_fg × rect_base` 时复位进度(update_value 0)；arc 的 style 只在 `arc_fg × arc_base` 时复位。
- **状态只有 btn 用**：pre/chk 仅 obj_btn 使用；其余控件均为 def 单状态。
- **层级色槽**：bar 的 color[4] 按 层级(base=0/edge=2/box=3/sha=3) + 部分(fg基础=1) 取色；
  arc 的 color[2] 按 部分(bg=0/fg=1) 取色，层级共用部分色。
