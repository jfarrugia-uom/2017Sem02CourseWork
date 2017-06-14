library(ggplot2)
# Visualisation R

#density plot of weighted odds.  Median odds of 1.48 (68% imputed probability) for win
density.test <- atp.elo.totals[atp.elo.totals$is_win==1, ]
density.test$year <- as.factor(as.numeric(density.test$year))
ggplot(density.test[density.test$year%in%c('2010','2011','2012','2013','2014','2015','2016'),], aes(x=weighted_odds)) + geom_density(aes(group=year, colour=year))+
  scale_x_continuous(trans='log2') + labs(title="Density plot of winning weighted odds by year", x="Weighted Odds (Log base 2)", y="Density")+
  theme(plot.title = element_text(hjust = 0.5))

median(density.test$weighted_odds)
mean(density.test$weighted_odds)

# outlier odds per tourney
outlier.test <- 
  atp.elo.totals %>%
  filter(is_win==1) %>%
  mutate(weighted_odds_log = log2(weighted.odds)) %>%
  sample_n(1000) %>%
  as.data.frame

op <- par(mar=c(7,4,4,2))
boxplot(outlier.test$weighted_odds_log ~ as.factor(outlier.test$tourney_level),
        ylab="Weighted Odds (log 2)", 
        horizontal=FALSE,
        las=2,
        main="Weighted Odds (log2) Outliers by Tourney Level"
)
rm(op)
rm(atp_elo)
rm(tennis.pe.all)

# comparison of odds to win outcome rate
odds.win.analysis <-
  atp.elo.totals %>%
  group_by(year, weighted_odds, is_win) %>%
  summarise(sum_matches = n()) %>%
  spread(is_win, sum_matches) %>%
  as.data.frame


colnames(odds.win.analysis) <- c("year", "weighted_odds", "matches_lost", "matches_won")
odds.win.analysis[is.na(odds.win.analysis$matches_lost), ]$matches_lost <- 0
odds.win.analysis[is.na(odds.win.analysis$matches_won), ]$matches_won <- 0

odds.win.analysis$win_rate <- mapply(function(x, y) { round(x / (x+y),2)}, odds.win.analysis$matches_won, odds.win.analysis$matches_lost)
ggplot(odds.win.analysis, aes(x = log2(weighted_odds), y = win_rate)) + 
  geom_point(size=0.4) + geom_smooth() + 
  facet_wrap(~year, ncol = 4) +
  labs(x="Weighted Odds (log2)", y="Win Rate") +
  ggtitle("Relationship of odds to win rate (2008-2017)") +
  theme(plot.title = element_text(hjust = 0.5)) 


# stacked chart showing volume matched for top 4 selection against rest of field in Grand Slam competitions
volume.agg.selection <-
  atp.elo.totals %>%
  select(year, tourney_name, tourney_level, selection, total_volume) %>%
  filter(tourney_level=='Grand Slams', year%in%c(2015:2016)) %>%
  group_by(year, tourney_name, selection) %>%
  summarise(grand_volume = sum(total_volume)) %>%
  as.data.frame


slam.2016.total_volume <-
  volume.agg.selection %>%
  group_by(year, tourney_name) %>%
  mutate(greatest.four= ifelse( selection %in% c('Federer', 'Nadal', 'Djokovic', 'Murray'),selection,"Other")) %>%
  as.data.frame %>%
  group_by(year, tourney_name, greatest.four) %>%
  summarise(betting_volume = sum(grand_volume)) %>%
  as.data.frame

slam.2016.total_volume$year <- as.factor(slam.2016.total_volume$year)
slam.2016.total_volume$greatest.four <- as.factor(slam.2016.total_volume$greatest.four)
slam.2016.total_volume$tourney_name <- as.factor(slam.2016.total_volume$tourney_name)    


slam.2016.total_volume$greatest.four <- factor(slam.2016.total_volume$greatest.four, levels=c('Other', 'Nadal', 'Federer', 'Murray', 'Djokovic'))
slam.2016.total_volume$tourney_name <- factor(slam.2016.total_volume$tourney_name, levels=c('Australian Open', 'Roland Garros', 'Wimbledon', 'Us Open'))

cbPalette <- c("#999999", "#E69F00", "#56B4E9", "#009E73", "#F0E442", "#0072B2", "#D55E00", "#CC79A7")
ggplot() + 
  geom_bar(aes(y=betting_volume, x=tourney_name, fill=greatest.four), data=slam.2016.total_volume, stat="identity", width=0.5) +
  scale_fill_manual(values=cbPalette) + scale_y_continuous(labels=comma) +
  labs(x="Tournament", y="Betting Volume (€)") +
  theme(axis.text.x = element_text(angle = 90, hjust = 1)) +
  facet_grid(~year) +
  ggtitle("Proportion of volume matched for top 4 players against rest of field in Grand Slams") +
  theme(plot.title = element_text(hjust = 0.5)) 

# plot average bet size as it changes with weighted odds
avg.bet.size <- 
  atp.elo.totals %>%
  select(year, weighted_odds, total_volume, total_bets) %>%
  group_by(year, weighted_odds) %>%
  summarise(total_volume=sum(total_volume), total_bets=sum(total_bets)) %>%
  mutate(avg_bet=total_volume/total_bets) %>%
  filter(year%in%c(2015,2016)) %>%
  as.data.frame()


p1 <- ggplot(avg.bet.size %>%
               group_by(weighted_odds) %>%
               summarise(avg_bet=mean(avg_bet))
             , aes(x=1/weighted_odds, y=avg_bet))
p1 + geom_line(color="red") + labs(x="Imputed probability 1/weighted odds", y="Average bet size (€)") +
  ggtitle("Relationship between event probability and bet size (2015-2016)") +
  theme(plot.title = element_text(hjust = 0.5)) 

# plot heat map of draws attracting most matched bets

# first find selections who attracted highest volume of bets in my dataset 
top.volume.players <-
  (atp.elo.totals %>%
     select(selection, total_volume) %>%
     group_by(selection) %>%
     summarise(sum_volume=sum(total_volume)) %>%
     filter(min_rank(desc(sum_volume)) <= 10) %>%
     as.data.frame)$selection

# now we'll come up with draws where any two of these players are matched up
# we'll order the draws alphabetically such that Federer vs Nadal and Nadal vs Federer can be grouped, their combined volume
# aggregated
pop.players.matches <-
  atp.elo.totals %>%
  select(player, opponent, total_volume) %>%
  filter(player %in% top.volume.players & opponent %in% top.volume.players) %>%
  mutate(player.1=mapply(function(x,y) { ifelse(x >= y, y,x)},player,opponent),
         opponent.1=mapply(function(x,y) {ifelse(x>=y, x, y)},player,opponent))  %>%
  group_by(player.1, opponent.1) %>%
  summarise(sum_volume=sum(total_volume)) %>%
  arrange(player.1) %>%
  as.data.frame

# finally, plot heatmap 
ggplot(pop.players.matches, aes(opponent.1, player.1)) +
  geom_tile(aes(fill = sum_volume), colour = "black") +
  scale_fill_gradient(low = "white", high = "steelblue", labels=comma) +
  labs(x="Player",y="Opponent") +
  ggtitle("Hottest draws in tennis based on matched betting volume")+
  theme(plot.title = element_text(hjust = 0.5)) 


# Frequency analysis of Elo probabilities segregated by tournament level
# Due to significant overlap, perhaps it would be best to avoid Challenger, Futures tournaments
# Elo is not much better than a coin toss in those cases
qplot(selection_elo_prediction, data=atp.elo.totals, geom="density", 
      fill=as.factor(is_win), alpha=I(0.5), 
      facets = (~as.factor(tourney_level)), 
      xlab="Elo Probability", ylab="Density", main="Frequency Disitrubtion of Elo Probabilites per Tourney Level ") + 
  theme(plot.title = element_text(hjust = 0.5)) 

# filter out matches more recent than 1st July 2016.  We will test our strategy on those matches
atp.elo.totals.test <-
  atp.elo.totals %>%
    filter(event_schedule_date < '2016/7/1')

atp.elo.totals.valid <-
  atp.elo.totals %>%
  filter(event_schedule_date >= '2016/7/1')

atp.elo.totals.valid %>%
  group_by(tourney_level) %>%
  summarise(cnt=n())

# same plot but using my test dataset
qplot(selection_elo_prediction, data=atp.elo.totals.test,
      geom="density", fill=as.factor(is_win), alpha=I(0.5), 
      facets = (~as.factor(tourney_level)),
      xlab="Elo Probability", ylab="Density", main="Frequency Distribution of Elo Probabilites per Tourney Level ") + 
  theme(plot.title = element_text(hjust = 0.5)) 

# Explore impact of Elo variance from imputed probablity derived from weighted odd
elo.variance <-
  atp.elo.totals.test %>%
  filter(tourney_level %in% c('250 or 500', 'Davis Cup', 'Tour Finals', 'Grand Slams', 'Masters'),
         (selection_elo_prediction - imputed_probability) > 0,
         selection_elo_prediction >= 0.6
  ) %>%
  mutate(elo_variance = round(abs(selection_elo_prediction - imputed_probability), 2)) %>%
  select(year, event_id, is_win, selection_elo_prediction, elo_variance) %>%
  as.data.frame

elo.variance$variance_range <- cut(elo.variance$elo_variance, seq(0, 1, 0.1), right=FALSE)
elo.variance$is_win <- as.factor(elo.variance$is_win)
# outcome of wins and losses from dataset - roughly 53% of selections were winning ones
atp.elo.totals.test %>%
  group_by(is_win) %>%
  summarise(cnt=n())

# filtering selections having Elo prediction >= 0.6 yields a improved ratio of wins (74%)
elo.variance %>%
  group_by(is_win) %>%
  summarise(cnt=n())

ggplot(data=elo.variance, aes(x=variance_range, fill=is_win)) + geom_bar(position="fill", width=0.7) +
  labs(x="Elo Variance Range", y="Proportion of Match Outcomes") +
  ggtitle("Impact of Elo Variance on Observed Match Outcome") +
  theme(plot.title = element_text(hjust = 0.5)) 

# probably wise to avoid betting on an outcome if deviations from odds is too wide - Betfair market prices 
# reflect risk accurately in general, although upsets do happen

