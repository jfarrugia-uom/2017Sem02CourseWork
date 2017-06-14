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
  

betting.set <-
atp.elo.totals.valid %>%
  filter(tourney_level %in% c('250 or 500', 'Davis Cup', 'Tour Finals', 'Grand Slams', 'Masters'),
         selection_elo_prediction >= 0.6, selection_elo_prediction - imputed_probability > 0) %>%
  select(year, event_id, event_schedule_date, player, opponent, selection, weighted_odds, imputed_probability, selection_elo_prediction, is_win) %>%
  arrange(event_schedule_date) %>%
  as.data.frame

head(betting.set,15)
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
  filter(year==2016, player=="Silva", opponent=="Benjasupawan") %>%
  

atp.matches.elo %>% 
  filter(year==2014,  player=="Cilic"|opponent=="Baghdatis")

atp.elo.totals %>% 
  filter(event_id==101443185)

atp_elo.2008 %>% 
  filter(year(tourney_start_date)==2015, player_surname=="Fish", opponent_surname=="Lopez")

tennis.matches %>%
  filter(event_id==115128311)

duplicate.remove %>%
  filter(player=="Gasquet"|opponent=="Gasquet", year(event_schedule_date)==2015, event_schedule_date>='2015/6/29')

tennis.tidy %>%
  filter(event_id==125419057)

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


