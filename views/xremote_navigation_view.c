/*!
 *  @file flipper-xremote/views/xremote_navigation_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Navigation page view callbacks and infrared functionality.
 */

#include "xremote_navigation_view.h"
#include "../xremote_app.h"


static void xremote_navigation_view_draw_vertical(Canvas* canvas, XRemoteViewModel* model)
{
    XRemoteAppContext *app_ctx = model->context;

    xremote_canvas_draw_button(canvas, model->up_pressed, 23, 30, XRemoteIconArrowUp);
    xremote_canvas_draw_button(canvas, model->down_pressed, 23, 72, XRemoteIconArrowDown);
    xremote_canvas_draw_button(canvas, model->left_pressed, 2, 51, XRemoteIconArrowLeft);
    xremote_canvas_draw_button(canvas, model->right_pressed, 44, 51, XRemoteIconArrowRight);
    xremote_canvas_draw_button(canvas, model->ok_pressed, 23, 51, XRemoteIconOk);
    xremote_canvas_draw_button(canvas, model->back_pressed, 2, 95, XRemoteIconBack);

    if (app_ctx->app_settings->exit_behavior == XRemoteAppExitPress)
        canvas_draw_icon(canvas, 22, 107, &I_Hold_Text_17x4);
}

static void xremote_navigation_view_draw_horizontal(Canvas* canvas, XRemoteViewModel* model)
{
    XRemoteAppContext *app_ctx = model->context;

    xremote_canvas_draw_button(canvas, model->up_pressed, 23, 2, XRemoteIconArrowUp);
    xremote_canvas_draw_button(canvas, model->down_pressed, 23, 44, XRemoteIconArrowDown);
    xremote_canvas_draw_button(canvas, model->left_pressed, 2, 23, XRemoteIconArrowLeft);
    xremote_canvas_draw_button(canvas, model->right_pressed, 44, 23, XRemoteIconArrowRight);
    xremote_canvas_draw_button(canvas, model->ok_pressed, 23, 23, XRemoteIconOk);
    xremote_canvas_draw_button(canvas, model->back_pressed, 70, 33, XRemoteIconBack);

    if (app_ctx->app_settings->exit_behavior == XRemoteAppExitPress)
        canvas_draw_icon(canvas, 90, 45, &I_Hold_Text_17x4);
}

static void xremote_navigation_view_draw_callback(Canvas* canvas, void* context)
{
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppContext *app_ctx = model->context;
    ViewOrientation orientation = app_ctx->app_settings->orientation;
    const char *exit_str = xremote_app_context_get_exit_str(app_ctx);

    XRemoteViewDrawFunction xremote_navigation_view_draw_body;
    xremote_navigation_view_draw_body = orientation == ViewOrientationVertical ?
        xremote_navigation_view_draw_vertical : xremote_navigation_view_draw_horizontal;

    xremote_canvas_draw_header(canvas, orientation, "Navigation");
    xremote_navigation_view_draw_body(canvas, model);
    xremote_canvas_draw_exit_footer(canvas, orientation, exit_str);
}

static void xremote_navigation_view_process(XRemoteView* view, InputEvent* event)
{
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel* model,
        {
            XRemoteAppContext* app_ctx = xremote_view_get_app_context(view);
            XRemoteAppExit exit = app_ctx->app_settings->exit_behavior;
            model->context = app_ctx;

            if (event->type == InputTypePress)
            {
                if (event->key == InputKeyUp)
                {
                    model->up_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_UP);
                }
                else if (event->key == InputKeyDown)
                {
                    model->down_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_DOWN);
                }
                else if (event->key == InputKeyLeft)
                {
                    model->left_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_LEFT);
                }
                else if (event->key == InputKeyRight)
                {
                    model->right_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_RIGHT);
                }
                else if (event->key == InputKeyOk)
                {
                    model->ok_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_OK);
                }
            }
            else if (event->type == InputTypeShort &&
                    event->key == InputKeyBack &&
                    exit == XRemoteAppExitHold)
            {
                model->back_pressed = true;
                xremote_view_send_ir(view, XREMOTE_COMMAND_BACK);
            }
            else if (event->type == InputTypeLong &&
                    event->key == InputKeyBack &&
                    exit == XRemoteAppExitPress)
            {
                model->back_pressed = true;
                xremote_view_send_ir(view, XREMOTE_COMMAND_BACK);
            }
            else if (event->type == InputTypeRelease)
            {
                if (event->key == InputKeyUp) model->up_pressed = false;
                else if (event->key == InputKeyDown) model->down_pressed = false;
                else if (event->key == InputKeyLeft) model->left_pressed = false;
                else if (event->key == InputKeyRight) model->right_pressed = false;
                else if (event->key == InputKeyOk) model->ok_pressed = false;
                else if (event->key == InputKeyBack) model->back_pressed = false;
            }
        },
        true);
}

static bool xremote_navigation_view_input_callback(InputEvent* event, void* context)
{
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    XRemoteAppContext* app_ctx = xremote_view_get_app_context(view);
    XRemoteAppExit exit = app_ctx->app_settings->exit_behavior;

    if (event->key == InputKeyBack &&
        event->type == InputTypeShort &&
        exit == XRemoteAppExitPress) return false;
    else if (event->key == InputKeyBack &&
        event->type == InputTypeLong &&
        exit == XRemoteAppExitHold) return false;

    xremote_navigation_view_process(view, event);
    return true;
}

XRemoteView* xremote_navigation_view_alloc(void* app_ctx)
{
    XRemoteView *view = xremote_view_alloc(app_ctx,
        xremote_navigation_view_input_callback,
        xremote_navigation_view_draw_callback);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel* model,
        {
            model->context = app_ctx;
            model->up_pressed = false;
            model->down_pressed = false;
            model->left_pressed = false;
            model->right_pressed = false;
            model->back_pressed = false;
            model->ok_pressed = false;
        },
        true
    );

    return view;
}