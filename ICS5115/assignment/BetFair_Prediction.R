# split data set between testing and validation
atp.elo.totals.test <-
  atp.elo.totals %>%
  filter(event_schedule_date < '2016/7/1')

# here is my validation set
atp.elo.totals.valid <-
  atp.elo.totals %>%
  filter(event_schedule_date >= '2016/7/1')

# Kelly Criterion wager size
kelly_wager <- function(probability, odds) {
  round(((probability * odds) -1 ) / (odds -1),2)
}

# Fixed exposure stake
fixed <- function(probability, odds) {
  0.1
}

# Betting set returns my betting picks.  Irrespective of actual win or loss, I consider these to be value bets:
# the Elo probability is greater than 0.6, and Elo is returning an edge over the imputed "conventional wisdom" 
# probability the market returns.
# We can choose between atp.elo.totals.value and atp.elo.totals.test sets 
betting.set <-
  atp.elo.totals.valid %>%
  #atp.elo.totals.test %>%
  filter(tourney_level %in% c('250 or 500', 'Davis Cup', 'Tour Finals', 'Grand Slams', 'Masters'),
         selection_elo_prediction >= 0.6, selection_elo_prediction - imputed_probability > 0) %>%
  select(year, event_id, tourney_level, event_schedule_date, 
         player, opponent, selection, weighted_odds, imputed_probability, selection_elo_prediction, is_win) %>%
  arrange(event_schedule_date) %>%
  as.data.frame

# calculate percentage of wins from our set.  As expected, slighly more than half of the Betfair selections win; slightly less than
# half lose.
atp.elo.totals.test %>%
  filter(tourney_level %in% c('250 or 500', 'Davis Cup', 'Tour Finals', 'Grand Slams', 'Masters')) %>%
  group_by(is_win) %>%
  summarise(cnt=n()) %>%
  spread(is_win,cnt ) %>%
  mutate(perc_win = `1` / (`1` + `0`))

# using our Elo predictor and selecting events that provide an edge over the BetFair market makers, we calculate the
# percentage of winning matches.   For tourney levels excluding Future and Challenger series, we get 68% correct pick.
betting.set %>%
  group_by(is_win) %>%
  summarise(cnt=n()) %>%
  spread(is_win,cnt ) %>%
  mutate(perc_win = `1` / (`1` + `0`))

# mean odds in betting set
mean(betting.set$weighted_odds)

# simulate betting run but don't account for bets placed on same date - assume that outcome of match is known
# before placing next bet
simulate_roll_simple <- function(df, col_name) {
  x <- numeric(nrow(df))        
  x[1] <- 100
  
  for (i in seq_len(nrow(df))) {
    wager <- df[i, col_name] * ifelse(i==1, 100, x[i-1])
    won <- ifelse(df[i, "is_win"] == 1, wager * df[i,]$weighted_odds, 0)
    x[i] <- ifelse(i==1, 100, x[i-1]) +  (won - wager)
  }
  x
}

calc_roi <- function(df, col_name) {
  x <- numeric(nrow(df))        
  x[1] <- 100
  wager <- numeric(nrow(df))        
  wager[1] <- 0
  won <- numeric(nrow(df))
  won[1] <- 0
  
  for (i in seq_len(nrow(df))) {
    wager[i] <- df[i, col_name] * ifelse(i==1, 100, x[i-1])
    won[i] <- ifelse(df[i, "is_win"] == 1, wager[i] * df[i,]$weighted_odds, 0)
#    print(paste(wager[i], won[i], sep=" "))
    x[i] <- ifelse(i==1, 100, x[i-1]) +  (won[i] - wager[i])
  }
  roi.df <- data.frame(bank = x, wager = wager, won = won)
  roi.df
}

# this function first generates a random sample from my validation data, calculates the wager size and then 
# simulates the actual bets.  The result is returned as a data frame.  "ldply" is then responsible to collate the various
# attempt runs in one large data frame.  The latter can then be used to plot, measure averages etc.
place_bets <- function(attempt, bet_set, sample_size, wager_function, limit_exposure) {
  bet.test <- bet_set %>%
    select(event_id, tourney_level, event_schedule_date, weighted_odds, imputed_probability, selection_elo_prediction, is_win) %>%
    # Here I'm filtering on Grand Slam matches only
    #filter(tourney_level=="Grand Slams") %>%
    mutate(wager = sapply(wager_function(selection_elo_prediction, weighted_odds), min, limit_exposure)) %>%
    sample_n(sample_size) %>%
    as.data.frame %>%
    arrange(event_schedule_date)
  #print(bet.test)
  
  attempt_label <- rep(paste("Attempt", attempt, sep=" "), sample_size)
  bet_count <- seq_len(sample_size)
  tmp.df <- calc_roi(bet.test, "wager")  
  
  df <- data.frame(attempt = attempt_label, bet_count = bet_count, bank = tmp.df$bank, wager = tmp.df$wager, won = tmp.df$won)
  df
}

# this function runs my bets using two different strategies: kelly and fixed exposure.  The latter bets 10% of the bank irrespective
# of edge; the former takes into account estimated probability and odds and determine the wager accordingly.  
place_bets_strategy <- function(attempt, bet_set, sample_size, limit_exposure) {
  bet.test <- bet_set %>%
    select(event_id, tourney_level, event_schedule_date, weighted_odds, imputed_probability, selection_elo_prediction, is_win) %>%
    filter(tourney_level=="Grand Slams") %>%
    mutate(kelly_wager = sapply(kelly_wager(selection_elo_prediction, weighted_odds), min, limit_exposure),
           fixed_wager = 0.1
           ) %>%
    sample_n(sample_size) %>%
    as.data.frame %>%
    arrange(event_schedule_date)
  #print(bet.test)
  attempt_label <- rep(paste("Attempt", attempt, sep=" "), sample_size)
  wager_strategy <- rep(c("kelly_wager", "fixed_wager"), rep(sample_size, 2))
  bet_count <- rep(seq_len(sample_size), 2)
  bank_roll <- unlist(lapply(c("kelly_wager", "fixed_wager"), function(x){ simulate_roll_simple(bet.test, x)}) )
  df <- data.frame(attempt = attempt_label, wager_strategy = wager_strategy, bet_count = bet_count, bank = bank_roll)
}


# set number of samples
n_sample <- 100
# run 10 attempts - bet a maximum of 10%.  Less if the edge is not sufficiently wide
df <- ldply(1:50, place_bets, betting.set, n_sample, kelly_wager, 0.1)


# calculate average bank roll after betting over all attempts
df %>%
  filter(bet_count == n_sample) %>%
  summarise(avg_bank = mean(bank))

# calculate mean roi over all attempts
df %>% 
  group_by(attempt) %>%
  summarise(roi = (sum(won) - sum(wager)) / sum(wager)) %>%
  summarise(mean(roi))

# plot each attempt in its own facet
qplot(x=bet_count, y=bank, data=df, geom="line",facets=(~attempt), xlab="Bet #", ylab = "Bank (€)", main="Bank Roll Projection" )

df <- ldply(1:100, place_bets_strategy, betting.set, n_sample, 0.5)
df %>%
  group_by(wager_strategy) %>%
  filter(bet_count == n_sample) %>%
  summarise(avg_bank = mean(bank))

# plot each attempt in its own facet; plot each bank roll in a different colour based on strategy
qplot(x=bet_count, y=bank, data=df, geom="line",color=wager_strategy, facets=(~attempt), xlab="Bet #", ylab = "Bank (€)", main="Bank Roll Projection" )
