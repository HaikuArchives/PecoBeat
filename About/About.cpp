/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <Entry.h>
#include <Font.h>
#include <Message.h>
#include <Mime.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Path.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>
#include <String.h>
#include <View.h>
#include <Window.h>

#include "About.h"
#include "URLView.h"

const int32	MSG_ABOUT_OK = 'AbOK';

class	AboutView : public BView {

public:

		AboutView(BRect frame, const char *programm_name, const char *release, const char *body_text, 
		const char* button_text, const char *site, const char *email, const char *sitename, const char *emailname);

void	Draw(BRect updateRect);
void	MessageReceived(BMessage *msg);
void	AttachedToWindow();
		~AboutView();
		
private:
	BBitmap			*fLogo, *fMuster, *fKopf, *fIcon;
	BBitmap			*fMailIcon, *fSiteIcon;
	BRect			fMailRect, fSiteRect;
	float			fBaseLine;
	const char		*fProgrammName, *fRelease, *fButtonText, *fSite, *fMail, *fSiteName, *fMailName;
	BString			fBodyText;
};

class AboutWindow : public BWindow {
	public:
		AboutWindow( BRect frame, const char *fenstertitel, const char *programm_name, 
			const char *release, const char *body_text, const char *button_text, 
			const char *site, const char *sitename, const char *email, const char *emailname)

			: 	BWindow( frame, fenstertitel, B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 
				B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS) {
			AboutView	*view = new AboutView(Bounds(), programm_name, release, body_text, button_text, site, sitename, email, emailname);
			AddChild(view);
			Show();
		}
		virtual bool	QuitRequested() { Hide(); return false; }
};

AboutView::AboutView(BRect frame, const char *programm_name, const char *release, const char *body_text, 
	const char* button_text, const char *site, const char *sitename, const char *email, const char *emailname)
	: 	BView(frame, NULL, B_FOLLOW_ALL, B_WILL_DRAW|B_DRAW_ON_CHILDREN), fLogo(0), fMuster(0), fKopf(0),
		fIcon(0), fMailIcon(0), fSiteIcon(0), fProgrammName(programm_name), fRelease (release), 
		fButtonText (button_text), fSite(site), fMail(email), fSiteName(sitename), fMailName(emailname), fBodyText(body_text) {

	// Resourcen holen
	BResources		*resourcen = be_app->AppResources();
	size_t			size;
	BMessage		message;
	char			*buf;
	
	// Hintergrund
	buf = (char *)resourcen->LoadResource('BBMP', "hgpattern", &size);
	message.Unflatten(buf);
	fMuster = new BBitmap(&message);

	// Hintergrund
	buf = (char *)resourcen->LoadResource('BBMP', "about:oben", &size);
	message.Unflatten(buf);
	fKopf = new BBitmap(&message);

	// Pecora-Logo
	buf = (char *)resourcen->LoadResource('BBMP', "about:logo", &size);
	message.Unflatten(buf);
	fLogo = new BBitmap(&message);

	// Programm-Icon
	app_info	myAppInfo;
	be_app->GetAppInfo(&myAppInfo);
	
	BEntry		myEntry = BEntry(&myAppInfo.ref, true);
	BNode 		myNode = BNode(&myEntry);
	BNodeInfo	myNodeInfo(&myNode);
	
	fIcon = new BBitmap(BRect(0,0,31,31), B_CMAP8 );
	myNodeInfo.GetTrackerIcon(fIcon);

	// link-Icons holen;
	entry_ref	ref;

	char		Signatur[B_MIME_TYPE_LENGTH];
	fMailIcon = new BBitmap( BRect(0,0,15,15), B_CMAP8 );
	fSiteIcon = new BBitmap( BRect(0,0,15,15), B_CMAP8 );
	
	BMimeType("text/x-email").GetPreferredApp(Signatur);
	if ( BMimeType(Signatur).GetIcon(fMailIcon, B_MINI_ICON) != B_OK )
		if ( BMimeType(Signatur).GetIconForType("text/x-email", fMailIcon, B_MINI_ICON) != B_OK )
			BMimeType("text/x-email").GetIcon(fMailIcon, B_MINI_ICON);
	BMimeType(Signatur).GetAppHint(&ref);

	BMimeType("text/html").GetPreferredApp(Signatur);
	if ( BMimeType(Signatur).GetIcon(fSiteIcon, B_MINI_ICON) != B_OK )
		if ( BMimeType(Signatur).GetIconForType("text/html", fSiteIcon, B_MINI_ICON) != B_OK )
			BMimeType("text/html").GetIcon(fSiteIcon, B_MINI_ICON);
	BMimeType(Signatur).GetAppHint(&ref);

//	BRect			fMailRect, fSiteRect;, *fSite, *fEmail

	// Schrift voreinstellen
	BFont	font(be_plain_font);
	font.SetSize(11);
	SetFont(&font);

	SetViewColor(41, 64, 96);
}

AboutView::~AboutView() {
	delete fMuster;
	delete fLogo;
	delete fIcon;
	delete fKopf;
	delete fMailIcon;
	delete fSiteIcon;
}

void	AboutView::Draw(BRect updateRect) {
	DrawBitmapAsync(fKopf, fKopf->Bounds(), BRect(0, 0, Bounds().right, 60));

	SetDrawingMode(B_OP_ALPHA);
	for (int i = 0; i < 15; i++) {
		SetHighColor( 0, 0, 0, 60 - i*4 );
		StrokeLine(BPoint(0, Bounds().bottom - i), BPoint(Bounds().right, Bounds().bottom - i));
	}
	
	DrawBitmapAsync( fLogo, BPoint( Bounds().right - fLogo->Bounds().Width() - 10, 10) );

	SetHighColor( 0, 0, 0 );
	SetLowColor( 255, 255, 255 );

	BFont	font(be_plain_font);

	font.SetFamilyAndFace("Dutch801 Rm BT", B_ITALIC_FACE);
	font.SetSize(30);
	SetFont(&font);

	DrawString(fProgrammName, BPoint(10, 33));
	
	float string_laenge = font.StringWidth(fProgrammName);
	
	font = BFont(be_plain_font);

	font.SetSize( 11.0 );
	SetFont( &font );
	
	DrawString( fRelease, BPoint(string_laenge - font.StringWidth(fRelease) + 5, 50) );

	SetDrawingMode(B_OP_OVER);
	DrawBitmapAsync( fIcon, BPoint(10, 25) );
	SetDrawingMode(B_OP_COPY);

	SetLowColor( 55, 96, 139 );
	SetHighColor( 255, 255, 255 );

	if (fBodyText.Length()>0) {
		BString substring;
		int leftside = 0;
		int rightside = 0;
		
		for (int i = 0; ; i++) {
			if ((rightside = fBodyText.FindFirst("\n", leftside)) == B_ERROR)
				rightside = fBodyText.Length();
			
			fBodyText.CopyInto( substring, leftside, rightside - leftside);
			DrawString(substring.String(), BPoint(10, 80 + i * 14));
			
			leftside = rightside + 1;
			if (leftside >= fBodyText.Length()) break;
		}
	}
	
	SetDrawingMode(B_OP_OVER);
	DrawBitmap( fMailIcon, fMailRect.LeftTop() );
	SetDrawingMode(B_OP_COPY);

	SetDrawingMode(B_OP_OVER);
	DrawBitmap( fSiteIcon, fSiteRect.LeftTop() );
	SetDrawingMode(B_OP_COPY);
};

void	AboutView::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case MSG_ABOUT_OK:
		case B_QUIT_REQUESTED:
			Window()->Hide();
			break;
		default:
			BView::MessageReceived(msg);
	}
};

void AboutView::AttachedToWindow() {
	SetViewBitmap(fMuster);
	
	BRect	ButtonFrame(Bounds().Width() - be_plain_font->StringWidth(fButtonText) - 30, Bounds().bottom - be_plain_font->Size() - 24, Bounds().Width() - 12, Bounds().bottom - 20);
	if ( ButtonFrame.Width() < 80 ) ButtonFrame.left = ButtonFrame.right - 80;

	BButton		*okButton = new BButton(ButtonFrame, NULL, fButtonText, new BMessage(MSG_ABOUT_OK));

	okButton->MakeDefault(true);
	okButton->SetTarget(this);
	
	AddChild( okButton );
	
	BFont	font(be_plain_font);
	font.SetSize(11.0 );
	
	rgb_color weiss = { 255, 255, 255, 255 };
	rgb_color hblau = { 170, 200, 255, 128 };
	rgb_color dblau = { 10, 40, 90, 255 };

	fMailRect = BRect( 10, Bounds().bottom - 38, 300, Bounds().bottom - 22);
	fSiteRect = BRect( 10, Bounds().bottom - 20, 300, Bounds().bottom - 4);

	// E-Mail
	BString mailstring = BString("mailto:").Append(fMail);
	URLView *email = new URLView( fMailRect.OffsetByCopy(20,-2), "E-mail", fMailName, mailstring.String() );
	email->SetFontSize( 11.0 );
	email->SetColor( weiss );
	email->SetHoverColor( hblau );
	email->SetClickColor( dblau );
	email->SetUnderlineThickness( 0 );
	email->SetPopUpEnabled( false );
	email->AddAttribute( "META:name", "Werner Freytag" );
	email->AddAttribute( "META:url", fSite );
	AddChild( email );

	// URL
	URLView *web = new URLView( fSiteRect.OffsetByCopy(20, -2), "Web", fSiteName, fSite );
	web->SetFontSize( 11.0 );
	web->SetColor( weiss );
	web->SetHoverColor( hblau );
	web->SetClickColor( dblau );
	web->SetPopUpEnabled( false );
	web->SetUnderlineThickness( 0 );
	web->AddAttribute( "META:keyw", "Pecora PecoBeat" );
	AddChild( web );
};

void About(const char *fenstertitel, const char *programm_name, 
	const char *release, const char *body_text, 
	const char* button_text, const char *site, const char *sitename, 
	const char *email, const char *emailname) {

	static BWindow	*fAboutWindow = NULL;
	
	// Doppelter Aufruf? Fenster vorholen!
	if (fAboutWindow != NULL) {
		fAboutWindow->Lock();
		if (fAboutWindow->IsHidden()) fAboutWindow->Show();
		else fAboutWindow->Minimize(false);
		fAboutWindow->Unlock();
		return;
	}
	
	BRect	screenFrame = BScreen().Frame();
	BRect	frame(140, 150, 440, 340);
	frame.OffsetTo( (screenFrame.Width() - frame.Width()) / 2,
		(screenFrame.Height() - frame.Height()) / 3 );

	fAboutWindow = new AboutWindow(frame, fenstertitel, programm_name, release, body_text, button_text, site, sitename, email, emailname);
}
