% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/joint_surv_VA.R
\name{joint_ms_format}
\alias{joint_ms_format}
\title{Formats the Parameter Vector}
\usage{
joint_ms_format(object, par = object$start_val)
}
\arguments{
\item{object}{a joint_ms object from \code{\link{joint_ms_ptr}}.}

\item{par}{parameter vector to be formatted.}
}
\description{
Formats a parameter vector by putting the model parameters into a \code{list}
with elements for each type of parameter.
}
