procedure	identificateur	procedure
(	symbole	PARENTHESE_OUVRANTE
entier	mot_clef	entier
$a	identificateur	$a
,	symbole	VIRGULE
entier	mot_clef	entier
$b	identificateur	$b
)	symbole	PARENTHESE_FERMANTE
entier	mot_clef	entier
$c	identificateur	$c
;	symbole	POINT_VIRGULE
{	symbole	ACCOLADE_OUVRANTE
$c	identificateur	$c
=	symbole	EGAL
2	nombre	2
;	symbole	POINT_VIRGULE
retour	mot_clef	retour
(	symbole	PARENTHESE_OUVRANTE
$b	identificateur	$b
+	symbole	PLUS
$a	identificateur	$a
+	symbole	PLUS
5	nombre	5
+	symbole	PLUS
$c	identificateur	$c
)	symbole	PARENTHESE_FERMANTE
;	symbole	POINT_VIRGULE
}	symbole	ACCOLADE_FERMANTE
main	identificateur	main
(	symbole	PARENTHESE_OUVRANTE
)	symbole	PARENTHESE_FERMANTE
{	symbole	ACCOLADE_OUVRANTE
ecrire	mot_clef	ecrire
(	symbole	PARENTHESE_OUVRANTE
procedure	identificateur	procedure
(	symbole	PARENTHESE_OUVRANTE
1	nombre	1
,	symbole	VIRGULE
2	nombre	2
)	symbole	PARENTHESE_FERMANTE
)	symbole	PARENTHESE_FERMANTE
;	symbole	POINT_VIRGULE
}	symbole	ACCOLADE_FERMANTE
