/* Codebase Macros - AFKMud
 *
 * This should cover the following derivatives:
 *
 * AFKMud: Versions 2.0 and up.
 *
 * Other derivatives should work too, please submit any needed changes to imc@imc2.org and be sure
 * you mention it's for AFKMud so that the email can be properly forwarded to me. -- Samson
 */

#ifndef __IMC2CFG_H__
#define __IMC2CFG_H__

#define first_descriptor connection_list
#define CH_IMCDATA(ch)        ((ch)->pcdata->imcchardata)
#define CH_IMCLEVEL(ch)       ((ch)->level)
#define CH_IMCNAME(ch)        ((ch)->name)
#define CH_IMCTITLE(ch)       ((ch)->pcdata->title)
#define CH_IMCRANK(ch)        ("")
#define CH_IMCSEX(ch)         ((ch)->sex)

#endif
