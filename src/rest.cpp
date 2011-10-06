#include "rest.h"
#include "reciteword.h"
#include "skin.h"

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;


static void
on_rest_start_clicked ()
{
}

static void
on_rest_pause_clicked ()
{
}

static void
on_rest_return_clicked ()
{
	g_pReciteWord->rest->close ();
	g_pReciteWord->show ();
}

CRest::CRest ()
{
	skin_load_rest (g_pReciteWord->window);
}

CRest::~CRest ()
{
}

void
CRest::init ()
{
	g_pReciteWord->status = STATUS_REST;
	started = FALSE;
}

void
CRest::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->rest.rest.p[0]);

	start_button.create (g_pReciteWord->fixed,
			     Skin->rest.stop_button.x[0],
			     Skin->rest.stop_button.y[0],
			     Skin->rest.stop_button.p[0],
			     Skin->rest.stop_button.p[1],
			     Skin->rest.stop_button.p[2],
			     (GdkPixmap *) NULL,
			     on_rest_start_clicked);
	pause_button.create (g_pReciteWord->fixed,
			     Skin->rest.pause_button.x[0],
			     Skin->rest.pause_button.y[0],
			     Skin->rest.pause_button.p[0],
			     Skin->rest.pause_button.p[1],
			     Skin->rest.pause_button.p[2],
			     Skin->rest.pause_button.p[3],
			     on_rest_pause_clicked);
	return_button.create (g_pReciteWord->fixed,
			      Skin->rest.return_button.x[0],
			      Skin->rest.return_button.y[0],
			      Skin->rest.return_button.p[0],
			      Skin->rest.return_button.p[1],
			      Skin->rest.return_button.p[2],
			      (GdkPixmap *) NULL,
			      on_rest_return_clicked);

	on_rest_start_clicked ();
}

void
CRest::close ()
{
	start_button.destroy ();
	pause_button.destroy ();
	return_button.destroy ();
}
