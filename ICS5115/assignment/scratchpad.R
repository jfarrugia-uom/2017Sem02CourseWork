library(dplyr)
library(ggplot2)
library(scales)

unique(tennis.pe.all$SCHEDULED_OFF)
unique(tennis.pe.all$FULL_DESCRIPTION)
unique(tennis.pe.all$EVENT)
names(tennis.pe.all)
names(tennis.df)

summary(tennis.pe.all)


nr.of.appearances <- aggregate(x = tennis.pe.all.year$EVENT_ID, 
                               by = list(unique.values = cut(tennis.pe.all.year$DT.ACTUAL_OFF, breaks='year')), 
                               FUN = length)

nr.of.appearances <- aggregate(x = test.sample$opponent,
                               by = list(unique.values = test.sample$opponent), 
                               FUN = length)



# find average elo reading for winning matches
median(atp.elo.totals[atp.elo.totals$is_win==1, "selection_elo_prediction"])


atp.elo.totals.test %>%
  group_by(tourney_level, is_win) %>%
  summarise(avg=mean(selection_elo_prediction))
  


atp.elo.totals.test %>%
  filter(tourney_level %in% c('250 or 500', 'Davis Cup', 'Tour Finals', 'Grand Slams', 'Masters'),
         is_win == 0, selection_elo_prediction >= 0.6, selection_elo_prediction > imputed_probability) %>%
  count()
  

kellyWager <- function(probability, odds) {
  round(((probability * odds) -1 ) / (odds -1),2)
}

betting.set <-
atp.elo.totals.test %>%
  filter(tourney_level %in% c('250 or 500', 'Davis Cup', 'Tour Finals', 'Grand Slams', 'Masters'),
         selection_elo_prediction >= 0.6, selection_elo_prediction - imputed_probability > 0) %>%
  select(year, event_id, tourney_level, event_schedule_date, 
         player, opponent, selection, weighted_odds, imputed_probability, selection_elo_prediction, is_win) %>%
  arrange(event_schedule_date) %>%
  as.data.frame


nrow(atp.elo.totals.valid)
nrow(betting.set)

min(0.1, kellyWager(0.6, 2))






    
    
summary(bet.test)

length(unique(atp.elo.totals.valid$event_id))

atp.elo.totals.test %>%
  filter(selection_elo_prediction >= 0.6, abs(selection_elo_prediction - imputed_probability) <= 0.3) %>%
  group_by(is_win) %>%
  summarise(cnt=n())


elo.variance %>%
  filter(elo_variance <= 0.3) %>%
  group_by(is_win) %>%
  summarise(cnt=n())
  
summary(elo.variance$elo_variance)


atp.elo.totals %>%
  filter(selection_elo_prediction - imputed_probability <= -0.8, is_win ==1, tourney_level == '250 or 500') %>%
  head

atp.elo.totals %>%
  filter(player=="Nadal", opponent=="Federer") %>%
  summarise(sum=sum(total_volume)) +
  
  atp.elo.totals %>%
  filter(player=="Federer", opponent=="Nadal") %>%
  summarise(sum=sum(total_volume))
   

sum(atp.elo.totals$total_volume  )

mean(atp.elo.totals[atp.elo.totals$is_win==0,]$selection_elo_prediction)

tennis.tidy %>%
  filter(year==2017, player=="Dimitrov", opponent=="Istomin") %>%
  

atp.matches.elo %>% 
  filter(year==2013,  player_surname=="Ferrer", opponent_surname=="Wawrinka")

atp.elo.totals %>% 
  filter(event_id==100452258)

atp_elo.2008 %>% 
  filter(year(tourney_start_date)==2014, player_surname=="Ferrer", opponent_surname=="Wawrinka")

tennis.matches %>%
  filter(event_id==112262226)

duplicate.remove %>%
  filter(player=="Gasquet"|opponent=="Gasquet", year(event_schedule_date)==2015, event_schedule_date>='2015/6/29')

tennis.tidy %>%
  filter(event_id==112262226)

atp.elo.totals %>%
  filter(tourney_level=="Futures") %>%
  sample_n(20)

tennis.pe.all.year %>%
  group_by(year) %>%
  summarise(cnt=n())

summary(test.sample.2)summary(test.sample.2)
head(atp.matches.elo)
head(atp_elo.2008)
head(atp.elo.totals)
head(tennis.matches)
head(weighted.odds)
head(density.test)
head(totals)
head(odds.win.analysis)
head(tennis.tidy)
head(slam.2016.total_volume)
head(elo.variance)
head(variance.win.summ)

colnames(atp.matches.elo)
colnames(atp.elo.totals)
colnames(tennis.matches)
colnames(weighted.odds)
colnames(tennis.tidy)
colnames(totals)

nrow(weighted.odds)
nrow(totals)

summary(weighted.odds)
summary(slam.2015.total_volume)
summary(elo.variance)


summary(atp.matches.elo$days_elapsed)

sd(atp.matches.elo$days_elapsed)

atp.matches.elo %>%
  filter(days_elapsed > 30)



atp.elo.totals %>% 
  filter(is_win == 1) %>% 
  summarise(mean(imputed_probability))

1-pbinom(13, 20, 0.5)

pt(1.76, 499, lower.tail=FALSE)

qt(0.95, 499)

ifelse(runif(2000, 0, 1) > 0.5, 'H', 'T')
