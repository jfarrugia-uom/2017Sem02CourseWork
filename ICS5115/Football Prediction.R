# compute probability of win,loss, draw between Team A and Team B

# use ELO rating to compute probability of win for Team A and Team B
# 1/ (1 + 10^(ELO B - ELO A)/400)
#Goals for the Home team:
#  if Proba < 0.5: Home Goals = 0.2 + 1.1*sqrt(Proba/0.5)
#  else: Home Goals = 1.69 / (1.12*sqrt(2 -Proba/0.5)+0.18)

#  Goals for the Away team:
#    if Proba < 0.8: Away goals = -0.96 + 1/(0.1+0.44*sqrt((Proba+0.1)/0.9))
#  else: Away goals = 0.72*sqrt((1 - Proba)/0.3)+0.3

# This should give us the expected value for Team A and Team B respectively
# Next plug these into poisson prob distribution function to compute 
# scores for P(T=0, B=0) P(T=1, B=0) etc... up to 5 goals
# All calculations courtesy of Lars Schiefler (http://clubelo.com)

# set directory
#getwd()
#setwd("/home/jfarrugia/CourseWork/2017Sem02CourseWork/ICS5115")
#elo <- read.csv("elo.in", header=TRUE, sep = "\t")
#summary(elo)

# to get elo of team do:
#elo[elo$Club=="Lazio",]$Elo
#elo[elo$Club=="Sassuolo",]$Elo

matchup <- c('Palermo', 'Cagliari')
if (nrow(elo[elo$Club %in% matchup, ]) < 2) {
  stop("ERROR! Failed to match both teams")
}

prob.win = 1 / 
  ( 1 + 10^( (elo[elo$Club==matchup[2],]$Elo - elo[elo$Club==matchup[1],]$Elo) / 400) )

#  Goals for the Home team:
#  if Proba < 0.5: Home Goals = 0.2 + 1.1*sqrt(Proba/0.5)
#  else: Home Goals = 1.69 / (1.12*sqrt(2 -Proba/0.5)+0.18)
home.goals <- function(probility.team.a) {
  if (probility.team.a < 0.5) {
    0.2 + ( 1.1 * sqrt(probility.team.a/0.5))
  } else {
    1.69 / (1.12 * sqrt(2 - probility.team.a/0.5) + 0.18)
  }
}
#  Goals for the Away team:
#    if Proba < 0.8: Away goals = -0.96 + 1/(0.1+0.44*sqrt((Proba+0.1)/0.9))
#  else: Away goals = 0.72*sqrt((1 - Proba)/0.3)+0.3
away.goals <- function(probability.team.a) {
  if (probability.team.a < 0.8) {
    -0.96 + 1 / ( 0.1 + 0.44 * sqrt((probability.team.a + 0.1) / 0.9 ))
  } else {
    0.72 * sqrt( (1 - probability.team.a) / 0.3) + 0.3
  }
}

home.expected <- home.goals(prob.win)
away.expected <- away.goals(prob.win)

# now generate poisson predictions based on home and away expected values
prob.score <- function (home, away) {
  round(dpois(home, home.expected), 5) * round(dpois(away, away.expected), 5)  
}

away.goals <- 0:5; names(away.goals) <- away.goals
home.goals <- 0:5; names(home.goals) <- home.goals
score.matrix <- outer(home.goals, away.goals, FUN = "prob.score")
# get most likely score-line
most.likely <- which(score.matrix == max(score.matrix), arr.ind = TRUE, useNames=TRUE)-1

# sum lower triangular matrix for probability of a win 
home <- round(sum(score.matrix[lower.tri(score.matrix, diag=FALSE)]), 2)
# sum upper triangular matrix for prob of loss
away <- round(sum(score.matrix[upper.tri(score.matrix, diag=FALSE)]),2 )
draw <- 1- (home + away)
c(home, draw, away)
most.likely
prob.win