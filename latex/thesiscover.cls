\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{thesiscover}[2017/04/11 Bachelor Thesis Cover Template UTCN]

\LoadClass[12pt, a4paper]{article}

%-------------------------------------------------------------------------------
%                3rd party packages
%-------------------------------------------------------------------------------
% Needed to make fixed length table
\RequirePackage{array}
% Needed to handle list environment
\RequirePackage{enumitem}
% Needed to handle text alignment
\RequirePackage{ragged2e}
% Needed to configure page layout
\RequirePackage{geometry}
% Needed to make header & footer effeciently
\RequirePackage{fancyhdr}
% Needed to manage colors
\RequirePackage{xcolor}
% Needed to use \ifxetex-\else-\fi statement
\RequirePackage{ifxetex}
% Needed to use \if-\then-\else statement
\RequirePackage{xifthen}
% Needed to use a toolbox of programming tools
\RequirePackage{etoolbox}
% Needed to change line spacing in specific environment
\RequirePackage{setspace}
% Needed to manage fonts
\ifxetex
  \RequirePackage[quiet]{fontspec}
  % To support LaTeX quoting style
  \defaultfontfeatures{Ligatures=TeX}
\else
  \RequirePackage[T1]{fontenc}
  % Replace by the encoding you are using
  \RequirePackage[utf8]{inputenc}
\fi
% Needed to manage math fonts
\RequirePackage{unicode-math}
% Needed to use icons from font-awesome
% (https://github.com/posquit0/latex-fontawesome)
% \RequirePackage{fontawesome}
% Needed to deal a paragraphs
\RequirePackage{parskip}
% Needed to deal hyperlink
\RequirePackage{hyperref}
\hypersetup{
  pdftitle={},
  pdfauthor={},
  pdfsubject={},
  pdfkeywords={},
  colorlinks=false,
  allbordercolors=white
}
\RequirePackage{graphicx}
\RequirePackage{chngcntr}
\RequirePackage{caption}
\RequirePackage{mathtools}
\RequirePackage{tabularx}
\RequirePackage{tabulary}
\RequirePackage{environ}
\RequirePackage{threeparttable}
\RequirePackage{multicol}


%-------------------------------------------------------------------------------
%                Configuration for layout
%-------------------------------------------------------------------------------
%% Page Layout
% Configure page margins with geometry
\geometry{left=2.5cm, headheight=120pt, top=2.17in, headsep=24pt, right=2.5cm, bottom=2.5cm, footskip=1.17cm}

%-------------------------------------------------------------------------------
%                Configuration for fonts
%-------------------------------------------------------------------------------
\newfontfamily\defaultfont{Caladea}
\newfontfamily\titlefont{FreeSerif}
\newfontfamily\headerfont{Carlito}

%-------------------------------------------------------------------------------
%                Configuration for styles
%-------------------------------------------------------------------------------
% Gray-scale colors
\definecolor{white}{HTML}{FFFFFF}
\definecolor{black}{HTML}{000000}
\definecolor{darkgray}{HTML}{333333}
\definecolor{gray}{HTML}{5D5D5D}
\definecolor{lightgray}{HTML}{999999}

\newcommand*{\titlestyle}[1]{{\fontsize{24pt}{1em}\scshape\titlefont\selectfont #1 \par}}
\newcommand*{\headerstyle}[1]{{\fontsize{10pt}{1em}\bfseries\headerfont\selectfont #1 \par}}
\newcommand*{\footerstyle}[1]{{\fontsize{24pt}{1em}\bfseries\defaultfont\selectfont #1 \par}}
\newcommand*{\defaultstyle}[1]{{\fontsize{12pt}{1em}\defaultfont\selectfont #1 \par}}
\newcommand*{\smallstyle}[1]{{\fontsize{11pt}{1em}\defaultfont\selectfont #1 \par}}
\newcommand*{\bigstyle}[1]{{\fontsize{14pt}{1em}\defaultfont\selectfont #1 \par}}

%-------------------------------------------------------------------------------
%                Configuration for paragraphs
%-------------------------------------------------------------------------------
\setlength{\parskip}{6pt}
\linespread{1.0}
\renewcommand{\baselinestretch}{1.1}
%\setlength{\parindent}{5ex}

%-------------------------------------------------------------------------------
%                Configuration for header and footer
%-------------------------------------------------------------------------------
%\setlength\headheight{111pt}

\pagestyle{fancy}% Change page style to fancy
\fancyhf{}% Clear header/footer

\renewcommand{\headrulewidth}{0pt}

% Set offset to each header and offset
\fancyhfoffset{0em}

\fancyhead[C]{\includegraphics[width=\linewidth]{antet.png} \\
			  \headerstyle{FACULTY OF AUTOMATION AND COMPUTER SCIENCE}}
%\fancyfoot[C]{\footerstyle{2017}}

%-------------------------------------------------------------------------------
%                Commands for setting variables
%-------------------------------------------------------------------------------
\newcommand*{\name}[2]{\def\@fname{#1} \def\@lname{#2}}

\newcommand*{\supervisor}[3]{\def\@sufname{#2} \def\@sulname{#3} \def\@sutitle{#1}}

\newcommand*{\thesistitle}[1]{\def\@thtitle{#1}}
\newcommand*{\thesisproblem}[1]{\def\@thproblem{#1}}
\newcommand*{\thesiscontent}[1]{\def\@thcontent{#1}}
\newcommand*{\thesisplace}[1]{\def\@thplace{#1}}
\newcommand*{\thesisadvisors}[1]{\def\@thadvisors{#1}}
\newcommand*{\thesisemissiondate}[1]{\def\@themissiondate{#1}}
\newcommand*{\thesisdeliverydate}[1]{\def\@thdeliverydate{#1}}

\newcommand*{\idcard}[2]{\def\@idseries{#1} \def\@idnumber{#2}}
\newcommand*{\CNP}[1]{\def\@cnp{#1}}
\newcommand*{\examsession}[1]{\def\@exsession{#1}}
\newcommand*{\declarationsigndate}[1]{\def\@declsigndate{#1}}

\newcommand*{\summaryrequirements}[1]{\def\@sumrequirements{#1}}
\newcommand*{\summarysolutions}[1]{\def\@sumsolutions{#1}}
\newcommand*{\summaryresults}[1]{\def\@sumresults{#1}}
\newcommand*{\summaryverification}[1]{\def\@sumverification{#1}}
\newcommand*{\summarycontribution}[1]{\def\@sumcontribution{#1}}
\newcommand*{\summarysources}[1]{\def\@sumsources{#1}}


%-------------------------------------------------------------------------------
%                Commands for generating the cover
%-------------------------------------------------------------------------------

\newcommand{\makecover}{
\vspace*{3.5cm}
\begin{center}
	\titlestyle{\@thtitle}
	\bigbreak
	DIPLOMA THESIS \\
\end{center}

\vfill
\hspace{5.9cm}
\begin{minipage}{\textwidth}
Author: \textbf{\@fname \space \uppercase{\@lname}}  \\
\bigbreak
Scientific supervisor: \textbf{\@sutitle \space \@sufname \space \uppercase{\@sulname}} 
\vspace{60pt}

\end{minipage}

\begin{center}
\footerstyle{2024}
\end{center}

\newpage

\hspace{8.05cm} \smallstyle{Approved,} 

\begin{multicols}{2}
\smallstyle{DEAN} 
\smallstyle{\textbf{Prof.dr.ing. Liviu MICLEA}} 

\smallstyle{HEAD OF AUTOMATION DEPARMENT} 
\smallstyle{\textbf{Prof.dr.ing. Honoriu VĂLEAN}} 
\end{multicols}

\vspace{36pt}

\begin{center}
Author: \textbf{\@fname \space \uppercase{\@lname}}
\bigbreak
\bigbreak
\textbf{\bigstyle{\@thtitle}}
\end{center}

\bigbreak
\bigbreak

\begin{enumerate}
\item \textbf{The problem: } \@thproblem

\bigbreak

\item \textbf{Content: } \@thcontent

\bigbreak

\item \textbf{Place of documentation: } \@thplace

\bigbreak

\item \textbf{Advisors:  } \@thadvisors

\bigbreak

\item \textbf{Thesis emission date:   } \@themissiondate

\bigbreak

\item \textbf{Thesis delivery date:   } \@thdeliverydate
\end{enumerate}

\bigbreak
\bigbreak
\bigbreak

\hspace{0.95cm}
\begin{minipage}{\linewidth}
Author signature \underline{\hspace{10.27cm}}

\bigbreak
\bigbreak

Scientific supervisor signature \underline{\hspace{8cm}}
\end{minipage}

\newpage
\begin{center}
\textbf{Declaration of authenticity}
\end{center}

\bigbreak

\hspace{5ex} I, the undersigned, \textbf{\@fname \space \uppercase{\@lname}}, identified by the CI/BI with series \@idseries, number \@idnumber, CNP \@cnp, author of the thesis: 

\textbf{\@thtitle} 

conceived for the final graduation examination 

at the \textbf{Faculty of Automation and Computer Science}, 

the Systems Engineering specialization in English, 

of the Technical University of Cluj-Napoca, 

\@exsession \space session, 

declare on my own responsibility that this thesis is the result of my own effort and \mbox{intellectual} activity, based on my own research and on information obtained from the sources cited both in the text and in the bibliography.

\hspace{5ex} I declare that this thesis does not contain plagiarized fragments, and that the bibliographic sources were used in conformity with the Romanian and international legislation regarding ownership rights.

\hspace{5ex} I also declare that this thesis has not been previously submitted to any other university for a higher degree.

\hspace{5ex} In the situation in which this declaration is proven to be false, I will endure all the consequences and administrative sanctions, consisting in the cancellation of the graduation exam.

\bigbreak
\bigbreak
\bigbreak

\hspace{5ex}
\begin{minipage}{\linewidth}
\begin{multicols}{2}
	Date \\
	\smallbreak
	\underline{\qquad \@declsigndate \qquad}
	
	\@fname \space \uppercase{\@lname} \\
	\smallbreak
	\underline{\hspace{3.3cm}}
\end{multicols}

\end{minipage}


\newpage
\begin{center}
\textbf{\bigstyle{SUMMARY}}

of the Diploma Thesis:\\
\bigskip
\textbf{\bigstyle{\@thtitle}}
\end{center}

\bigbreak
\bigbreak

Author: \textbf{\@fname \space \uppercase{\@lname}}

Scientific supervisor: \textbf{\@sutitle \space \@sufname \space \uppercase{\@sulname}}

\bigbreak

\begin{enumerate}[leftmargin=*,labelsep=5ex]
	\item Requirements: \space \@sumrequirements
	\bigbreak
	\item Solutions: \space \@sumsolutions
	\bigbreak
	\item Obtained results: \space \@sumresults
	\bigbreak
	\item Tests and verification: \space \@sumverification
	\bigbreak
	\item Personal contribution: \space \@sumcontribution
	\bigbreak
	\item Bibliographical sources: \space \@sumsources
\end{enumerate} 

\bigbreak
\bigbreak
\bigbreak

\begin{minipage}{\linewidth}
Author signature \underline{\hspace{10.27cm}}

\bigbreak
\bigbreak

Scientific supervisor signature \underline{\hspace{8cm}}
\end{minipage}
}
