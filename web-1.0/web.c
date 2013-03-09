/*                    Minimalist Web Browser 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Copyright 2013 Ralph Glass                                           */

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <stdlib.h>
#include <string.h>

static int               window_count = 0;
static gboolean          javascript = FALSE;
static WebKitWebSettings *settings;

/* G_CALLBACK(functions) */

static void hoveringOverLink( WebKitWebView* webView ,
                              gchar*         title ,
                              gchar*         uri ,
                              gchar*         uriEntry )
{
        if (!(uri==NULL)){
                gtk_entry_set_text( GTK_ENTRY( uriEntry ), uri );}
}

static void destroy( GtkWidget* widget, GtkWidget* window )
{
        if (window_count < 2)
                gtk_main_quit();
        window_count--;
}

static void closeView( WebKitWebView* webView, GtkWidget* window )
{
        gtk_widget_destroy( window );
}

static void goBack( GtkWidget* window, WebKitWebView* webView )
{
        webkit_web_view_go_back( webView );
}

static void goHome( GtkWidget* window, WebKitWebView* webView )
{
        /* load start page (change to your favourite) */
        webkit_web_view_load_uri( webView, "http://www.google.com" ); 
}

static void downloadRequested( WebKitWebView*  webView,
                               WebKitDownload* download, 
                               GtkEntry*       user_data_entry )
{
        const char* uri = gtk_entry_get_text( user_data_entry );
        const char* homedir = getenv("HOME");
        const char* downloaddir = g_strjoin( NULL,   
                                             "--directory-prefix=",
                                             homedir, 
                                             "/Downloads", 
                                             NULL );
        /* fork download with wget */
        int r = 0;
        r = fork();
        if (r>0) {
                execl( "/usr/bin/wget", "wget", downloaddir, // download directory
                                                "--no-clobber", // already downloaded?
                                                "--adjust-extension", 
                                                "--content-disposition", // use suggested filenam 
                                                uri , 
                                                NULL); };
}

static void searchText( WebKitWebView* webView, gchar* searchString )
{
        webkit_web_view_unmark_text_matches( webView );
        webkit_web_view_search_text( webView, searchString, false, true, true );
        webkit_web_view_mark_text_matches( webView, searchString, false, 0 );
        webkit_web_view_set_highlight_text_matches( webView, true );
}

static void toggleJavascript( GtkWidget* window , WebKitWebView* webView )
{
        javascript=!javascript;
        g_object_set (G_OBJECT(settings), "enable-scripts", javascript , NULL);
        webkit_web_view_reload( webView );
}

static void activateEntry( GtkWidget* entry, gpointer gdata )
{
        WebKitWebView* webView = g_object_get_data( G_OBJECT( entry ), "webView" );
        const gchar* entry_str = gtk_entry_get_text( GTK_ENTRY( entry ));
        const gchar* search_str = "http://www.google.com/search?as_q="; 
        /* uri complete */
        if ( strncmp( entry_str , "www.", 4 ) == 0 ){ 
                gtk_entry_set_text( GTK_ENTRY( entry ),  
                                    g_strjoin( NULL, "http://", entry_str, NULL ) );
        /* search text on page if prefix / */
        } else if ( strncmp( entry_str, "/", 1) == 0 ){
                gchar* s = g_strdup( entry_str );
                s++; // remove prefix
                searchText( webView, s ); 
                return;
        /* search with google if no http head */
        } else if ( strncmp( entry_str, "http", 4 ) != 0 ){ 
                gtk_entry_set_text( GTK_ENTRY( entry ),  
                                    g_strjoin( NULL, search_str, entry_str, NULL ));
        }
        const gchar* uri = gtk_entry_get_text( GTK_ENTRY( entry ));
        g_assert( uri );
        g_assert( webView );
        webkit_web_view_load_uri( webView, uri );
}

static WebKitWebView* createWebView ( WebKitWebView*  parentWebView,
                                      WebKitWebFrame* frame,
                                      GtkEntry*       parentUriEntry)
{
        window_count++;
        const char* user_data;
        if (!parentUriEntry == 0)
                user_data = gtk_entry_get_text( parentUriEntry );
        
        GtkWidget*     window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
        GtkWidget*     vbox = gtk_vbox_new( FALSE, 0 );
        GtkWidget*     scrolledWindow = gtk_scrolled_window_new( NULL, NULL );
        GtkWidget*     uriEntry = gtk_entry_new();
        GtkWidget*     toolbar = gtk_toolbar_new();
        GtkToolItem*   item;
        WebKitWebView* webView = WEBKIT_WEB_VIEW( webkit_web_view_new() );

        g_object_set (G_OBJECT(settings), "enable-scripts", javascript, NULL); 
        g_object_set (G_OBJECT(settings), "enable-private-browsing", TRUE, NULL);
        webkit_web_view_set_settings( WEBKIT_WEB_VIEW(webView), settings);

        gtk_entry_set_text( GTK_ENTRY( uriEntry ), "http://" );

        item = gtk_tool_button_new_from_stock( GTK_STOCK_GO_BACK );
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Back" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK( goBack ), webView );

        item = gtk_tool_button_new_from_stock( GTK_STOCK_HOME );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1 );
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Home" );
        g_signal_connect( G_OBJECT( item ), "clicked", G_CALLBACK( goHome ), webView );

        item = gtk_toggle_tool_button_new_from_stock( GTK_STOCK_PROPERTIES );
        gtk_tool_button_set_label(GTK_TOOL_BUTTON( item ), "Javascript" );
        gtk_toolbar_insert( GTK_TOOLBAR( toolbar ), item, -1);
        gtk_toggle_tool_button_set_active( GTK_TOGGLE_TOOL_BUTTON( item ), javascript );
        g_signal_connect( G_OBJECT( item ), "toggled", G_CALLBACK( toggleJavascript ), webView );

        gtk_window_set_default_size( GTK_WINDOW( window ), 1024, 768 );
        g_object_set_data( G_OBJECT( uriEntry ), "webView", webView );

        gtk_container_add(  GTK_CONTAINER( scrolledWindow ), GTK_WIDGET( webView ) );
        gtk_box_pack_start( GTK_BOX( vbox ), toolbar, FALSE, FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( vbox ), uriEntry, FALSE, FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( vbox ), scrolledWindow, TRUE, TRUE, 0 );
        gtk_container_add(  GTK_CONTAINER( window ), vbox );

        g_signal_connect( window,      "destroy",             G_CALLBACK( destroy ),            window );
        g_signal_connect( webView ,    "close-web-view",      G_CALLBACK( closeView ),          window );
        g_signal_connect( uriEntry,    "activate",            G_CALLBACK( activateEntry ),      NULL );
        g_signal_connect( webView ,    "create-web-view",     G_CALLBACK( createWebView ),      uriEntry );
        g_signal_connect( webView ,    "hovering_over_link",  G_CALLBACK( hoveringOverLink ),   uriEntry );
        g_signal_connect( webView ,    "download-requested" , G_CALLBACK( downloadRequested ),  uriEntry );

        g_assert( user_data );
        if (!user_data)
                webkit_web_view_load_uri( webView, user_data );
        else
                webkit_web_view_load_uri( webView, "http://www.google.com" );

        gtk_widget_grab_focus( GTK_WIDGET( webView ) );
        gtk_widget_show_all( window );
        return webView;
}

/* main */

int main( int argc, char* argv[] )
{
        gtk_init( &argc, &argv );
        settings = webkit_web_settings_new();
        createWebView( NULL, NULL, NULL );
        gtk_main();
        return 0;
}
