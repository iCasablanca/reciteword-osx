#include "know.h"
#include "reciteword.h"
#include "skin.h"

extern CReciteWord *g_pReciteWord;
extern struct _Skin *Skin;


static void
on_know_return_clicked ()
{
	g_pReciteWord->know->close ();
	g_pReciteWord->show ();
}

CKnow::CKnow ()
{
	skin_load_know (g_pReciteWord->window);
}

CKnow::~CKnow ()
{
}

void
CKnow::init ()
{
	g_pReciteWord->status = STATUS_KNOW;
}

void
CKnow::show ()
{
	init ();

	SetBackPixmap (g_pReciteWord->fixed, Skin->know.know.p[0]);

	return_button.create (g_pReciteWord->fixed,
			      Skin->know.return_button.x[0],
			      Skin->know.return_button.y[0],
			      Skin->know.return_button.p[0],
			      Skin->know.return_button.p[1],
			      Skin->know.return_button.p[2],
			      (GdkPixmap *) NULL,
			      on_know_return_clicked);
}

void
CKnow::close ()
{
	return_button.destroy ();
}
