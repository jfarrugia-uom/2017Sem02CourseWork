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

# Fixed exposure
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

# size of betting.set 
nrow(betting.set)

# simulate betting run
simulate_roll_simple <- function(df) {
  x <- numeric(nrow(df))        
  x[1] <- 100
  
  for (i in seq_len(nrow(df))) {
    wager <- df[i, ]$wager * ifelse(i==1, 100, x[i-1])
    won <- ifelse(df[i, "is_win"] == 1, wager * df[i,]$weighted_odds, 0)
    print(won)
    x[i] <- ifelse(i==1, 100, x[i-1]) +  (won - wager)
  }
  x
}

# this function first generates a random sample from my validation data, calculates the wager size and then 
# simulates the actual bets.  The result is returned as a data frame.  "ldply" is then responsible to collate the various
# attempt runs in one large data frame.  The latter can then be used to plot, measure averages etc.
place_bets <- function(attempt, bet_set, sample_size, wager_function, limit_exposure) {
  bet.test <- bet_set %>%
    select(event_id, tourney_level, event_schedule_date, weighted_odds, imputed_probability, selection_elo_prediction, is_win) %>%
    #filter(tourney_level=="Grand Slams") %>%
    mutate(wager = sapply(wager_function(selection_elo_prediction, weighted_odds), min, limit_exposure)) %>%
    #mutate(wager = kellyWager(selection_elo_prediction, weighted_odds)) %>%
    #mutate(wager = 0.1) %>%
    sample_n(sample_size) %>%
    as.data.frame %>%
    arrange(event_schedule_date)
  
  print(bet.test)
  attempt_label <- rep(paste("Attempt", attempt, sep=" "), sample_size)
  bet_count <- seq_len(sample_size)
  bank_roll <- simulate_roll_simple(bet.test)  
  df <- data.frame(attempt = attempt_label, bet_count = bet_count, bank = bank_roll)
}

n_sample <- 10
df <- ldply(1:1, place_bets, betting.set, n_sample, kelly_wager, 0.1)
df %>%
  filter(bet_count == n_sample) %>%
  summarise(avg_bank = mean(bank))

df[df$bet_count==n_sample,]
qplot(x=bet_count, y=bank, data=df, geom="line",color=attempt )
 

bet.test %>%
  group_by(is_win) %>%
  summarise(cnt=n())

#df <- data.frame(attempt=seq_len(length(x)), bank=x, test=rep(c("test"), length(x)))


#to create data frame with columns as follows
# Attempt 
# Bet Count
# Bank Roll after each bet
